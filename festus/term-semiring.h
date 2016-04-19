// festus/term-semiring.h
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Copyright 2016 Google, Inc.
// Author: mjansche@google.com (Martin Jansche)
//
// \file
// Term algebra over the signature of star semirings.
//
// A TermSemiring instance represents terms as opaque handles. These handles
// can be manipulated via the usual semiring (or weight) operations, e.g.:
//
//   TermSemiring<...> sr;
//   auto one = sr.One();
//   auto x = sr.From(symbols->Find("x"));
//   auto one_plus_x = sr.OpPlus(one, x);
//   auto star = sr.OpStar(one_plus_one);
//
// In this example, the opaque handle 'star' encodes the term (One + "x")*.
//
// The class TermSemiring<> implements handles as plain old 64-bit integers
// (this is an implementation detail that could change at any moment). Using a
// POD type to represent handles has many advantages: once created, handles are
// thread-safe, can be copied and stored cheaply, put into containers, etc.
//
// One drawback is that 64-bit integers cannot by themselves represent
// arbitrarily deep expression graphs. Some form of additional storage is
// needed. The TermSemiring class template takes that additional storage in the
// form of a memoization helper class, or Memo. The latter is essentially a
// bijective map, which is used to pack handles into smaller integers.
//
// This design gives rise to a few caveats:
//
// * There is a choice of several Memo implementations which are appropriate in
//   different circumstances. In addition, client code can supply a custom Memo
//   class. If a TermSemiring object keeps state in the Memo, and if that state
//   is manipulated concurrently from multiple threads, some form of
//   synchronization is required. Using a SynchronizedMemo results in a
//   thread-safe TermSemiring object. Using an UnsynchronizedMemo means that
//   the TermSemiring object is not thread-safe; it then requires external
//   locking if accessed from multiple threads. A NoopMemo keeps no state and
//   is trivially thread-safe.
//
// * If a handle was memoized, it is generally not serializable. A handle only
//   remains meaningful as long as the TermSemiring object that created it is
//   alive, since its meaning generally depends on the state of the Memo.
//   (With a NoopMemo, there is no state, so this caveat does not apply.)
//
// In some situations (e.g. in tests), the stateless NoopMemo is sufficient.
// A TermSemiring<NoopMemo> object cannot keep state and relies entirely on
// 64-bit handles to represent terms via bit-packing. While the bit-level
// layout of a handle is described in TermSemiringBase, this is an
// implementation detail that can change at any moment without notice.
//
// The method ToGraph() turns an opaque handle into an expression graph that
// can be inspected and serialized, and which can outlive its originating
// TermSemiring object.

#ifndef FESTUS_TERM_SEMIRING_H__
#define FESTUS_TERM_SEMIRING_H__

#include <array>
#include <cstddef>
#include <mutex>
#include <ostream>
#include <unordered_map>
#include <vector>

#include <fst/compat.h>

#include "festus/expression.pb.h"
#include "festus/value-weight-singleton.h"

namespace festus {

// Non-template empty base class from which TermSemiring<> inherits.
class TermSemiringBase {
 public:
  typedef uint64 ValueType;

  // There is a hard limit of 60 bits on the size of the leaf payload. In
  // practice it is advisable to keep the leaf payload size at or below 31
  // bits (better yet: at or below 25 bits) to reduce memoization.
  static constexpr uint64 LeafLimit() { return 1ULL << 60; }

  // Creates a leaf term with the given payload. The payload value must be less
  // than kLeafLimit.
  static constexpr uint64 From(uint64 val) {
    return (val < LeafLimit()) ? ((val << 4) | kLeaf) : MakeError(kErrorLeaf);
  }

  static uint64 GetLeafPayload(uint64 x) {
    CHECK(IsLeaf(x));
    return GetLeafPayloadUnchecked(x);
  }

  static constexpr uint64 NoWeight() { return MakeError(kNoWeight); }
  static constexpr uint64 Zero() { return kZero; }
  static constexpr uint64 One() { return kOne; }
  static constexpr uint64 Quantize(uint64 x, float) { return x; }

  static constexpr uint64 OpMinus(uint64, uint64) {
    return MakeError(kErrorUndefinedMinus);
  }
  static constexpr uint64 OpDivide(uint64, uint64) {
    return MakeError(kErrorUndefinedDivide);
  }
  static constexpr uint64 Reciprocal(uint64) {
    return MakeError(kErrorUndefinedReciprocal);
  }

  static constexpr bool Member(uint64 x) { return !IsError(x); }

  // Tests if x is equal to Zero. If x was created bottom-up by a TermSemiring
  // object, this predicate is both necessary and sufficient for algebraic
  // equality with Zero, since Zero is only equal to itself. The term semiring
  // is assumed to be nontrivial, i.e. One != Zero; all leaves are distinct
  // from Zero; and, by construction, whenever a value equal to Zero is
  // created by a semiring operation, that operation returns a literal Zero.
  static constexpr bool NotZero(uint64 x) { return x != kZero; }

  // EqualTo is a sufficient condition for algebraic equality of x and y. This
  // means it is possible that EqualTo(x, y) returns false but x and y are in
  // fact equal under the semiring axioms. Testing accurately for equality
  // would entail building minimal DFAs, which can be expensive. In practice,
  // the TermSemiring is appropriate in a limited number of circumstances
  // where testing specific equalities (e.g. NotZero()) is sufficient.
  static constexpr bool EqualTo(uint64 x, uint64 y) {
    return !IsError(x) && x == y;
  }

  static constexpr bool ApproxEqualTo(uint64 x, uint64 y, float delta) {
    return EqualTo(x, y);
  }

 protected:
  // Least significant bits of a handle:
  //
  //       01  Times with two leaf nodes
  //       11  Times, general case
  //       10  binary semiring Plus
  //     0100  Star
  //     1100  unary KleenePlus (w+ == w w*)
  //     1000  Leaf
  //  00 0000  Zero
  //  01 0000  One
  //  1. 0000  Errors, including NoWeight
  //
  static constexpr uint8 kMask2  = 0x03;
  static constexpr uint8 kTimesL = 0x01;
  static constexpr uint8 kTimesG = 0x03;
  static constexpr uint8 kPlus   = 0x02;

  static constexpr uint8 kMask4  = 0x0F;
  static constexpr uint8 kStar   = 0x04;
  static constexpr uint8 kKPlus  = 0x0C;
  static constexpr uint8 kKPlusBit = kStar ^ kKPlus;
  static constexpr uint8 kLeaf   = 0x08;

  static constexpr uint8 kMask3  = 0x07;
  static constexpr uint8 kUnary  = kStar & kKPlus;

  static constexpr uint8 kMask6  = 0x3F;
  static constexpr uint8 kZero   = 0x00;
  static constexpr uint8 kOne    = 0x10;
  static constexpr uint8 kError  = 0x20;
  static constexpr uint8 kMaskE  = 0x2F;

  // Returns the payload of the given leaf term.
  static constexpr uint64 GetLeafPayloadUnchecked(uint64 x) { return x >> 4; }

  static constexpr uint64 PackingLimit(int8 total_bits) {
    return 1ULL << (total_bits - 1);
  }

  static constexpr bool IsBinary(uint64 x) { return x & kMask2; }
  static constexpr bool IsTimes (uint64 x) { return x & 0x1; }
  static constexpr bool IsPlus  (uint64 x) { return (x & kMask2) == kPlus; }

  static constexpr bool IsUnary (uint64 x) { return (x & kMask3) == kUnary; }
  static constexpr bool IsStar  (uint64 x) { return (x & kMask4) == kStar; }
  static constexpr bool IsKPlus (uint64 x) { return (x & kMask4) == kKPlus; }
  static constexpr bool IsLeaf  (uint64 x) { return (x & kMask4) == kLeaf; }

  static constexpr bool IsZero  (uint64 x) { return x == kZero; }
  static constexpr bool IsOne   (uint64 x) { return x == kOne; }
  static constexpr bool IsError (uint64 x) { return (x & kMaskE) == kError; }

  static const char *ErrorMessage(uint64 x) {
    CHECK(IsError(x));
    x >>= 6;
    if (x >= kNumErrors) {
      x = kUnspecifiedError;
    }
    return kErrorMessage[x];
  }

  enum Error {
    kNoWeight = 0,
    kErrorLeaf = 1,
    kErrorPack = 2,
    kErrorUnpack = 3,
    kErrorUndefinedMinus = 4,
    kErrorUndefinedDivide = 5,
    kErrorUndefinedReciprocal = 6,
    kErrorUndefinedStar = 7,
    kUnspecifiedError = 8,
    kNumErrors = 9,
  };

  static constexpr uint64 MakeError(Error code) {
    return (static_cast<uint64>(code) << 6) | kError;
  }

  static const std::array<const char *, kNumErrors> kErrorMessage;
};

const std::array<const char *, TermSemiringBase::kNumErrors>
TermSemiringBase::kErrorMessage{{
    "NoWeight",
    "leaf value too large",
    "Pack failed",
    "Unpack failed",
    "undefined Minus",
    "undefined Divide",
    "undefined Reciprocal",
    "undefined Star",
    "unspecified error",
}};

// Trivial memoization helper which keeps no state and does nothing.
// Highly thread-safe.
struct NoopMemo {
  static string Name() { return "noop"; }
  static constexpr bool Lookup(uint64 *) { return false; }
  static constexpr bool Memoize(uint64 *) { return false; }
  static constexpr std::size_t Size() { return 0; }
};

// Non-thread-safe (but thread-compatible) memoization helper.
// Requires external synchronization when accessed from multiple threads.
//
// Lookup() and Size() provide read-only access and are safe to use by multiple
// threads without synchronization, provided no other thread is simultaneously
// invoking Memoize() to mutate the underlying storage.
//
// This implementation can be used in a scenario where one thread accesses
// this object under an exclusive lock (externally held) to mutate it; after
// that work is complete, subsequent access is read-only. In that situation
// the read-only access, after the initial mutation is complete, can be done
// without locking.
class UnsynchronizedMemo {
 public:
  static string Name() { return "unsynchronized"; }

  bool Lookup(uint64 *index) const {
    if (*index < index_to_handle_.size()) {
      *index = index_to_handle_[*index];
      return true;
    }
    return false;
  }

  bool Memoize(uint64 *handle) const {
    auto iter = handle_to_index_.find(*handle);
    if (iter != handle_to_index_.end()) {
      *handle = iter->second;
    } else {
      auto index = index_to_handle_.size();
      handle_to_index_.emplace(*handle, index);
      index_to_handle_.emplace_back(*handle);
      *handle = index;
    }
    return true;
  }

  std::size_t Size() const {
    return index_to_handle_.size();
  }

 private:
  mutable std::unordered_map<uint64, uint64> handle_to_index_;
  mutable std::vector<uint64> index_to_handle_;
};

// Thread-safe memoization helper.
class SynchronizedMemo {
 public:
  static string Name() { return "synchronized"; }

  bool Lookup(uint64 *payload) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return memo_.Lookup(payload);
  }

  bool Memoize(uint64 *handle) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return memo_.Memoize(handle);
  }

  std::size_t Size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return memo_.Size();
  }

 private:
  mutable std::mutex mutex_;
  mutable UnsynchronizedMemo memo_;
};

// Property helper that specifies properties which can lead to algebraic
// simplifications. This struct specifies the properties of a freely-generated
// semiring, i.e. there are no additional algebraic identities beyond the
// semiring axioms.
struct FreeSemiringProperties {
  static string Name() { return "free"; }

  // In general, Times is not commutative.
  static constexpr bool kCommutative = false;

  // In general, Plus is not idempotent.
  static constexpr bool kIdempotent = false;

  // In general, the semiring operations are defined for all arguments.
  static constexpr bool UndefinedStar(const void *, uint64) {
    return false;
  }
};

// Property helper for semirings with idempotent Plus.
struct IdempotentSemiringProperties {
  static string Name() { return "idempotent"; }
  static constexpr bool kCommutative = false;
  static constexpr bool kIdempotent = true;

  static constexpr bool UndefinedStar(const void *, uint64) {
    return false;
  }
};

// Property helper for a commutative ring R (e.g. the field of Reals).
struct CommutativeRingProperties {
  static string Name() { return "commutative_ring"; }
  static constexpr bool kCommutative = true;
  static constexpr bool kIdempotent = false;

  // 1* is always undefined in rings.
  //
  // From the star axiom x* == 1 + x x* by subtraction in the additive group
  // we get x* - x x* == 1, and by distributivity (1 - x) x* == 1. But when
  // x == 1 this would mean 0 1* == 1, which is impossible if 1* is defined.
  template <class R>
  static constexpr bool UndefinedStar(const R *r, uint64 x) {
    return r->EqualTo(x, r->One());
  }
};

// Term algebra over the signature of star semirings.
//
// This class conforms to the signature expected by the ValueWeightSingleton
// adapter/façade, which can be used to turn this into an OpenFst weight class.
template <class Memo, class Props = FreeSemiringProperties>
class TermSemiring : public TermSemiringBase {
 public:
  using TermSemiringBase::ValueType;

  static string Name() {
    return Props::Name() + "_term_" + Memo::Name();
  }

  static constexpr bool Commutative() { return Props::kCommutative; }
  static constexpr bool Idempotent() { return Props::kIdempotent; }

  constexpr TermSemiring() : memo_() {}

  uint64 GetChild1(uint64 x) const {
    CHECK(IsBinary(x));  // Plus or Times
    return GetChild1Unchecked(x);
  }

  uint64 GetChild2(uint64 x) const {
    CHECK(IsBinary(x));  // Plus or Times
    return GetChild2Unchecked(x);
  }

  uint64 GetChild(uint64 x) const {
    CHECK(IsUnary(x));  // Star or KleenePlus
    return GetChildUnchecked(x);
  }

  uint64 OpPlus(uint64 x, uint64 y) const {
    if (IsError(x)) return x;
    if (IsError(y)) return y;
    if (Props::kIdempotent && x == y) return x;
    if (IsZero(x)) return y;
    if (IsZero(y)) return x;
    if (IsTimes(x) || IsOne(y)) {
      uint64 tmp = x;
      x = y;
      y = tmp;
    }
    if (IsTimes(y)) {
      uint64 child1 = GetChild1Unchecked(y);
      uint64 child2 = GetChild2Unchecked(y);
      if (child1 == x) {
        // x + [x c2] ==> x (1 + c2)
        return OpTimes(x, OpPlus(One(), child2));
      }
      if (child2 == x) {
        // x + [c1 x] ==> (1 + c1) x
        return OpTimes(OpPlus(One(), child1), x);
      }
    }
    if (IsOne(x)) {
      if (IsKPlus(y)) {
        // 1 + KleenePlus(w) ==> w*
        return y & ~kKPlusBit;  // clear the KleenePlus bit
      }
      if (IsStar(y) && (IsOne(GetChildUnchecked(y)) || Props::kIdempotent)) {
        // Generally: 1 + 1* ==> 1 + [1 1*] ==> 1*
        // In an idempotent semiring: 1 + w* ==> w*
        return y;
      }
    }
    return MakeBinaryOp(kPlus, x, y, true);
  }

  uint64 OpTimes(uint64 x, uint64 y) const {
    if (IsError(x)) return x;
    if (IsError(y)) return y;
    if (IsZero(x) || IsZero(y)) return Zero();
    if (IsStar(x) && GetChildUnchecked(x) == y) {
      // Star(y) y ==> KleenePlus(y)
      return x | kKPlusBit;  // set the KleenePlus bit
    }
    if (IsStar(y) && GetChildUnchecked(y) == x) {
      // x Star(x) ==> KleenePlus(x)
      return y | kKPlusBit;  // set the KleenePlus bit
    }
    if (IsOne(x)) return y;
    if (IsOne(y)) return x;
    if (IsLeaf(x) && IsLeaf(y)) {
      uint64 p, q;
      if (Props::kCommutative && x > y) {
        p = GetLeafPayloadUnchecked(y);
        q = GetLeafPayloadUnchecked(x);
      } else {
        p = GetLeafPayloadUnchecked(x);
        q = GetLeafPayloadUnchecked(y);
      }
      if (p < (1ULL << 31) && q < (1ULL << 31)) {
        return (q << 33) | (p << 2) | kTimesL;
      }
    }
    return MakeBinaryOp(kTimesG, x, y, Props::kCommutative);
  }

  uint64 OpStar(uint64 x) const {
    if (IsError(x)) return x;
    if (Props::UndefinedStar(this, x)) {
      return MakeError(kErrorUndefinedStar);
    }
    // At this point, Star(x) is known to be defined. By the star axiom,
    // Star(x) == One + x Star(x), whose subexpressions must be defined as
    // well. The only possible defined value for Star(Zero) is One.
    if (IsZero(x)) return One();
    if (!Pack(&x, PackingLimit(60))) {
      return MakeError(kErrorPack);
    }
    return (x << 4) | kStar;
  }

  uint64 Reverse(uint64 val) const {
    return Props::kCommutative ? val : ReverseAux(val);
  }

  std::ostream &Print(std::ostream &strm, uint64 val) const;

  // Returns a serializable, stand-alone ExpressionGraph that is fully
  // decoupled from this object and can outlive it.
  ExpressionGraph ToGraph(uint64 val) const {
    std::unordered_map<uint64, uint32> handle_to_node;
    ExpressionGraph graph;
    ToGraphAux(val, &handle_to_node, &graph);
    graph.set_root(graph.node().size() - 1);
    return graph;
  }

  // Returns the number of items stored in this object's Memo.
  std::size_t MemoSize() const { return memo_.Size(); }

 private:
  // Reverses a term in a non-commutative semiring.
  uint64 ReverseAux(uint64 val) const;

  void ToGraphAux(uint64 val,
                  std::unordered_map<uint64, uint32> *handle_to_node,
                  ExpressionGraph *graph) const;

  uint64 KleenePlus(uint64 x) const {
    if (IsError(x)) return x;
    if (Props::UndefinedStar(this, x)) {
      return MakeError(kErrorUndefinedStar);
    }
    // At this point, Star(x) is known to be defined. By the star axiom,
    // Star(x) == One + KleenePlus(x). Therefore KleenePlus(x) etc. are known
    // to be defined as well, and the only possible value for KleenePlus(Zero)
    // is Zero.
    if (IsZero(x)) return Zero();
    if (!Pack(&x, PackingLimit(60))) {
      return MakeError(kErrorPack);
    }
    return (x << 4) | kKPlus;
  }

  uint64 MakeBinaryOp(uint8 tag, uint64 x, uint64 y, bool commutative) const {
    if (commutative && x > y) {
      uint64 tmp = x;
      x = y;
      y = tmp;
    }
    // Least significant bits (tt is the 2-bit tag for binary operations):
    //
    //   0 tt  Half format, first child fits into 13 bits
    //   1 tt  Full format, first child fits into 30 bits
    //
    // Half format:
    //   unsigned child2 : 48;  // packed value of y
    //   unsigned child1 : 13;  // packed value of x
    //      uint8 format :  1;  // constant 1
    //      uint8 tag    :  2;  // kPlus or kTimesG
    //
    // Full format:
    //   unsigned child2 : 31;  // packed value of y
    //   unsigned child1 : 30;  // packed value of x
    //      uint8 format :  1;  // constant 0
    //      uint8 tag    :  2;  // kPlus or kTimesG
    //
    // If both children fit into 13 bits when packed, the Half format value
    // fits into 29 bits, i.e. can be packed into 30 bits. This means the Half
    // format can then be used as either child of a Full format expression.
    if (x < PackingLimit(30) && y < PackingLimit(31)) {
      if (x < PackingLimit(13)) {  // Half format
        return (Direct(y) << 16) | (Direct(x) << 3) | 0x4 | tag;
      } else {  // Full format
        return (Direct(y) << 33) | (Direct(x) << 3) | tag;
      }
    } else if (Pack(&x, PackingLimit(30)) && Pack(&y, PackingLimit(31))) {
      if (x < PackingLimit(13)) {  // Half format
        return (y << 16) | (x << 3) | 0x4 | tag;
      } else {  // Full format
        return (y << 33) | (x << 3) | tag;
      }
    } else {
      return MakeError(kErrorPack);
    }
  }

  uint64 GetChild1Unchecked(uint64 handle) const {
    if ((handle & kMask2) == kTimesL) {
      return From((handle >> 2) & ((1ULL << 31) - 1));
    }
    if (handle & 0x4) {  // Half format
      return Unpack((handle >> 3) & ((1ULL << 13) - 1));
    } else {  // Full format
      return Unpack((handle >> 3) & ((1ULL << 30) - 1));
    }
  }

  uint64 GetChild2Unchecked(uint64 handle) const {
    if ((handle & kMask2) == kTimesL) {
      return From(handle >> 33);
    }
    if (handle & 0x4) {  // Half format
      return Unpack(handle >> 16);
    } else {  // Full format
      return Unpack(handle >> 33);
    }
  }

  uint64 GetChildUnchecked(uint64 handle) const {
    handle >>= 4;
    return Unpack(handle);
  }

  // Packs a 64-bit handle into log2(limit) + 1 bits.
  //
  // If the given handle fits is less than the limit, use it directly, with
  // an added rightmost bit of 1.
  //
  // Otherwise memoize it and use the resulting index with an added rightmost
  // bit of 0.
  bool Pack(uint64 *handle, uint64 limit) const {
    if (*handle < limit) {
      *handle = Direct(*handle);  // Rightmost bit is set.
      return true;
    }
    if (memo_.Memoize(handle) && *handle < limit) {
      *handle <<= 1;   // Leave rightmost bit unset to signal memoized payload.
      return true;
    }
    return false;
  }

  static constexpr uint64 Direct(uint64 handle) {
    return (handle << 1) | 0x1;  // Set rightmost bit to signal direct payload.
  }

  // Unpacks the given payload into a 64-bit handle.
  //
  // If the rightmost bit is 1, the payload constitutes a packed handle
  // and can be returned directly (without the rightmost bit).
  //
  // Otherwise the payload (without the rightmost bit) represents an index
  // into the memo, where it will be looked up.
  uint64 Unpack(uint64 payload) const {
    bool direct = payload & 0x1;  // Value is direct iff rightmost bit is set.
    payload >>= 1;
    if (direct || memo_.Lookup(&payload)) {
      return payload;
    }
    return MakeError(kErrorUnpack);
  }

  const Memo memo_;
};

// Reverses a term by recursive traversal and swapping the order of the
// children of a Times expression.
template <class M, class P>
uint64 TermSemiring<M, P>::ReverseAux(uint64 x) const {
  switch (x & kMask2) {
    case kTimesL:
    case kTimesG:
      return OpTimes(ReverseAux(GetChild2Unchecked(x)),  // NB: switcheroo
                     ReverseAux(GetChild1Unchecked(x)));
    case kPlus:
      return OpPlus(ReverseAux(GetChild1Unchecked(x)),
                    ReverseAux(GetChild2Unchecked(x)));
    default:
      break;
  }
  CHECK_EQ(0, x & kMask2);
  switch (x & kMask4) {
    case kStar:
      return OpStar(ReverseAux(GetChildUnchecked(x)));
    case kKPlus:
      return KleenePlus(ReverseAux(GetChildUnchecked(x)));
    default:  // Zero, One, Leaf, or Error
      return x;
  }
}

template <class M, class P>
std::ostream &TermSemiring<M, P>::Print(std::ostream &strm, uint64 x) const {
  switch (x & kMask2) {
    case kTimesL:
    case kTimesG: {
      strm << "Times(";
      Print(strm, GetChild1Unchecked(x));
      strm << ", ";
      Print(strm, GetChild2Unchecked(x));
      return strm << ")";
    }
    case kPlus: {
      strm << "Plus(";
      Print(strm, GetChild1Unchecked(x));
      strm << ", ";
      Print(strm, GetChild2Unchecked(x));
      return strm << ")";
    }
    default:
      break;
  }
  CHECK_EQ(0, x & kMask2);
  switch (x & kMask4) {
    case kStar:
      strm << "Star(";
      Print(strm, GetChildUnchecked(x));
      return strm << ")";
    case kKPlus:
      strm << "KleenePlus(";
      Print(strm, GetChildUnchecked(x));
      return strm << ")";
    case kLeaf:
      return strm << "Leaf(" << GetLeafPayloadUnchecked(x) << ")";
    default:
      break;
  }
  CHECK_EQ(0, x & kMask4);
  switch (x & kMask6) {
    case kZero:
      return strm << "Zero";
    case kOne:
      return strm << "One";
    default:
      return strm << "Error(" << ErrorMessage(x) << ")";
  }
}

inline uint32 HandleToNode(
    const std::unordered_map<uint64, uint32> *h2n,
    uint64 handle) {
  auto iter = h2n->find(handle);
  CHECK_NE(iter, h2n->end());
  return iter->second;
}

template <class M, class P>
void TermSemiring<M, P>::ToGraphAux(
    uint64 x,
    std::unordered_map<uint64, uint32> *h2n,
    ExpressionGraph *graph) const {
  // Recursive DFS traversal to sort the graph into topological order. The
  // graph is known to be acyclic, so there is no need to mark nodes as
  // explored but unfinished in order to detect cycles.
  auto &graph_node = *graph->mutable_node();
  CHECK_EQ(graph_node.size(), h2n->size());
  auto iter = h2n->find(x);
  if (iter != h2n->end()) {
    const uint32 node = iter->second;
    CHECK_NE(graph_node.find(node), graph_node.end());
    auto &expression = graph_node[node];
    expression.set_multiple_parents(true);
    return;
  }
  if (IsBinary(x)) {
    ToGraphAux(GetChild1Unchecked(x), h2n, graph);
    ToGraphAux(GetChild2Unchecked(x), h2n, graph);
  } else if (IsUnary(x)) {
    ToGraphAux(GetChildUnchecked(x), h2n, graph);
  }
  CHECK_EQ(graph_node.size(), h2n->size());
  const uint32 finish_time = graph_node.size();
  VLOG(3) << "Finished node " << finish_time;
  (*h2n)[x] = finish_time;
  auto &expression = graph_node[finish_time];
  CHECK_EQ(graph_node.size(), h2n->size());

  // Populate the Expression proto message with the node information encoded in
  // the current handle.
  switch (x & kMask2) {
    case kTimesL:
    case kTimesG: {
      BinaryExpression *times = expression.mutable_times();
      times->set_child1(HandleToNode(h2n, GetChild1Unchecked(x)));
      times->set_child2(HandleToNode(h2n, GetChild2Unchecked(x)));
      return;
    }
    case kPlus: {
      BinaryExpression *plus = expression.mutable_plus();
      plus->set_child1(HandleToNode(h2n, GetChild1Unchecked(x)));
      plus->set_child2(HandleToNode(h2n, GetChild2Unchecked(x)));
      return;
    }
    default:
      break;
  }
  CHECK_EQ(0, x & kMask2);
  switch (x & kMask4) {
    case kStar:
      expression.mutable_star()->set_child(
          HandleToNode(h2n, GetChildUnchecked(x)));
      return;
    case kKPlus:
      expression.mutable_kleene_plus()->set_child(
          HandleToNode(h2n, GetChildUnchecked(x)));
      return;
    case kLeaf:
      expression.mutable_leaf()->set_payload(GetLeafPayloadUnchecked(x));
      return;
    default:
      break;
  }
  CHECK_EQ(0, x & kMask4);
  switch (x & kMask6) {
    case kZero:
      expression.set_zero(false);
      return;
    case kOne:
      expression.set_one(false);
      return;
    default:
      expression.set_error(ErrorMessage(x));
      return;
  }
}

template <class Props>
using BasicTermSemiring = TermSemiring<NoopMemo, Props>;

template <class Props>
using BasicTermWeightTpl = festus::ValueWeightSingleton<
  festus::BasicTermSemiring<Props>,
  festus::DefaultStaticInstance<festus::BasicTermSemiring<Props>>>;

}  // namespace festus

#endif  // FESTUS_TERM_SEMIRING_H__
