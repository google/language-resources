// festus/expression-node.h
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
// A node in a formal expression graph for star semirings.
//
// This is used to represent arbitrary formal expressions in star semirings. A
// node object represents one of the following kinds of expressions:
//
// * A designated invalid element, NoWeight.
// * The formal Zero and One elements of the semiring.
// * An arbitrary Leaf element.
// * The formal binary Plus, and Times operations of the semiring.
//   (Minus is included because existing OpenFst weight classes support it.)
// * The unary Star operation of the semiring.
//
// Nonterminal nodes have one or two children pointing to other nodes.

#ifndef FESTUS_EXPRESSION_NODE_H__
#define FESTUS_EXPRESSION_NODE_H__

#include <memory>
#include <ostream>
#include <utility>

#include <fst/compat.h>
#include <fst/weight.h>

#include "festus/expression.pb.h"

namespace festus {

// The template classes ExpressionNode<> and FormalWeight<> expect as their
// template parameter a class with a similar signature as in this example:
struct ExampleLeaf {
  // The storage type of leaf payloads in ExpressionNode<>.
  typedef string Leaf;

  // Returns a name for the leaf type. Used by FormalWeight<>::Type().
  static string TypeName() { return "example"; }

  // Serializes a leaf value to the protobuf message LeafValue.
  static void ToLeafValue(const string &val, festus::LeafValue *leaf_value) {
    leaf_value->set_bytes_value(val);
  }

  // Semiring properties, used by FormalWeight<>::Properties().
  // The values provided here are for illustration only.
  static uint64 Properties() { return fst::kSemiring | fst::kCommutative; }
};

template <class L>
class ExpressionNode {
 public:
  class Visitor;

  ExpressionNode() = default;

  explicit ExpressionNode(Expression::ExpressionCase c) : xcase_(c) {}

  template <typename... Args>
  ExpressionNode(Expression::ExpressionCase c, Args &&... args)
      : xcase_(c), leaf_(std::forward<Args>(args)...) {
  }

  ~ExpressionNode() = default;
  ExpressionNode(ExpressionNode &&) = default;
  ExpressionNode(const ExpressionNode &) = default;
  ExpressionNode &operator=(ExpressionNode &&) = default;
  ExpressionNode &operator=(const ExpressionNode &) = default;

  static std::shared_ptr<const ExpressionNode> NoWeight() {
    return kNoWeightNode;
  }

  // Returns the expression Zero.
  static std::shared_ptr<const ExpressionNode> Zero() { return kZeroNode; }

  // Returns the expression One.
  static std::shared_ptr<const ExpressionNode> One() { return kOneNode; }

  // Builds a leaf expression.
  template <typename... Args>
  static std::shared_ptr<const ExpressionNode> Leaf(Args &&... args) {
    return std::make_shared<const ExpressionNode>(
        Expression::kLeaf, std::forward<Args>(args)...);
  }

  // Computes the Plus expression by setting n1 = Plus(n1, n2).
  static void Plus(std::shared_ptr<const ExpressionNode> &n1,  // non-const
                   const std::shared_ptr<const ExpressionNode> &n2);

  // Computes the Minus expression by setting n1 = Minus(n1, n2).
  static void Minus(std::shared_ptr<const ExpressionNode> &n1,  // non-const
                    const std::shared_ptr<const ExpressionNode> &n2);

  // Computes the Times expression by setting n1 = Times(n1, n2).
  static void Times(std::shared_ptr<const ExpressionNode> &n1,  // non-const
                    const std::shared_ptr<const ExpressionNode> &n2);

  // Computes the Star expression by setting n = Star(n).
  static void Star(std::shared_ptr<const ExpressionNode> &n);  // non-const

  // Builds the reverse expression recursively by reversing the order of the
  // children of all Times expressions.
  static std::shared_ptr<const ExpressionNode> Reverse(
      const std::shared_ptr<const ExpressionNode> &node);

  // Tests conservatively for equality between this node and node n, with at
  // most shallow generic evaluations. A more rigorous test would require
  // expanding and evaluating both nodes. Since this is a formal semiring, we do
  // not know what the leaf nodes mean at this level of abstraction. In order to
  // compare two nodes without evaluating them, we rely on general properties of
  // semirings and we make a few additional simplifying assumptions:
  //
  // * NoWeight is not equal to anything, not even itself.
  // * Zero != One. (Otherwise the semiring would become trivial.)
  // * Zero != leaf != One.
  //   (Depending on the meaning we assign to leaves, a leaf might actually be
  //   equal to Zero or to One. E.g. if leaves are strings and Times is string
  //   concatenation, then One is equal to the empty string. Or if leaves are
  //   real-valued variables and Times and Plus are ordinary multiplication and
  //   addition, then a leaf may take on the value 0 or 1 depending on the
  //   evaluation environment.)
  // * Zero is equal to compound expressions (formed with Plus, Times, etc.)
  //   only as required by the semiring axioms. In particular:
  //   - Zero == Zero + Zero
  //   - Zero == x - x for leaves x
  //   - Zero == Zero * x == x * Zero for all x
  //   - Zero != Star(x) for all x
  // * One is equal to compound expressions only as required by the semiring
  //   axioms. In particular:
  //   - One == One + Zero == Zero + One
  //   - One == One - Zero
  //   - One == One * One
  //   - One == Star(Zero)
  // * Aside from comparisons with Zero and One, leaves will be compared for
  //   equality with other leaves, but not with compound expressions.
  // * Compound expressions will not be compared for equality.
  //
  // When a comparison cannot be carried out, a conservative value of false is
  // returned. This is sufficient to make many OpenFst algorithms work correctly
  // (an exception is ShortestDistance over cyclic FSTs).
  //
  // An optional second argument 'fast' controls whether the comparison is
  // carried out without recursion (uses recursion iff fast == false).
  bool IsEqual(const ExpressionNode &n, bool fast = false) const;

  // Auxiliary methods for equality comparisons:
  bool IsZeroRecursive() const;
  bool IsOneRecursive() const;

  bool NotMember() const { return xcase_ == Expression::EXPRESSION_NOT_SET;  }
  bool IsZero()    const { return xcase_ == Expression::kZero; }
  bool IsOne()     const { return xcase_ == Expression::kOne; }
  bool IsLeaf()    const { return xcase_ == Expression::kLeaf; }
  bool IsPlus()    const { return xcase_ == Expression::kPlus; }
  bool IsMinus()   const { return xcase_ == Expression::kMinus; }
  bool IsTimes()   const { return xcase_ == Expression::kTimes; }
  bool IsStar()    const { return xcase_ == Expression::kStar; }

  void Accept(Visitor *visitor) const;

 private:
  static const std::shared_ptr<const ExpressionNode> kNoWeightNode;
  static const std::shared_ptr<const ExpressionNode> kZeroNode;
  static const std::shared_ptr<const ExpressionNode> kOneNode;

  Expression::ExpressionCase xcase_ = Expression::kZero;
  typename L::Leaf leaf_;
  std::shared_ptr<const ExpressionNode> child1_;
  std::shared_ptr<const ExpressionNode> child2_;

  template <class K>
  friend void ToExpressionGraph(const ExpressionNode<K> &, ExpressionGraph *);
};

template <class L>
const std::shared_ptr<const ExpressionNode<L>> ExpressionNode<L>::kNoWeightNode{
  new ExpressionNode<L>(Expression::EXPRESSION_NOT_SET)};

template <class L>
const std::shared_ptr<const ExpressionNode<L>> ExpressionNode<L>::kZeroNode{
  new ExpressionNode<L>(Expression::kZero)};

template <class L>
const std::shared_ptr<const ExpressionNode<L>> ExpressionNode<L>::kOneNode{
  new ExpressionNode<L>(Expression::kOne)};

template <class L>
bool ExpressionNode<L>::IsZeroRecursive() const {
  switch (xcase_) {
    case Expression::EXPRESSION_NOT_SET:
      return false;
    case Expression::kZero:
      return true;
    case Expression::kOne:  // Zero != One in a nontrivial semiring.
      return false;
    case Expression::kLeaf:
      VLOG(2) << "Assuming that leaf " << leaf_ << " != Zero";
      return false;
    case Expression::kPlus:
      // This does not cover the case of (x + (0 - x)).
      return child1_->IsZeroRecursive() && child2_->IsZeroRecursive();
    case Expression::kMinus:
      return child1_->IsEqual(*child2_, false);
    case Expression::kTimes:
      return child1_->IsZeroRecursive() || child2_->IsZeroRecursive();
    case Expression::kStar:
      // This errs on the side of being cautious. E.g. in the real semiring as
      // defined in festus/real-weight.h, we have Star(±inf) == ∓0.
      return false;
    default:
      LOG(ERROR) << "expression case " << xcase_ << " not handled by "
                 << "IsZero()";
  }
  VLOG(2) << "Returning false by default from IsZero()";
  return false;
}

template <class L>
bool ExpressionNode<L>::IsOneRecursive() const {
  switch (xcase_) {
    case Expression::EXPRESSION_NOT_SET:
      return false;
    case Expression::kZero:  // Zero != One in a nontrivial semiring.
      return false;
    case Expression::kOne:
      return true;
    case Expression::kLeaf:
      VLOG(2) << "Assuming that leaf " << leaf_ << " != One";
      return false;
    case Expression::kPlus:
      return (child1_->IsOneRecursive() && child2_->IsZeroRecursive())
          || (child2_->IsOneRecursive() && child1_->IsZeroRecursive());
    case Expression::kMinus:
      return child1_->IsOneRecursive() && child2_->IsZeroRecursive();
    case Expression::kTimes:
      return child1_->IsOneRecursive() && child2_->IsOneRecursive();
    case Expression::kStar:
      return child1_->IsZeroRecursive();
    default:
      LOG(ERROR) << "expression case " << xcase_ << " not handled by "
                 << "IsOneRecursive()";
  }
  VLOG(2) << "Returning false by default from IsOne()";
  return false;
}

template <class L>
bool ExpressionNode<L>::IsEqual(const ExpressionNode<L> &n, bool fast) const {
  if (NotMember() || n.NotMember()) return false;
  if (this == &n) return true;
  // The most common case, comparison against plain Zero:
  if (IsZero() || n.IsZero()) {
    const auto &other = IsZero() ? n : *this;
    return fast ? other.IsZero() : other.IsZeroRecursive();
  }
  if (IsOne() || n.IsOne()) {
    const auto &other = IsOne() ? n : *this;
    return fast ? other.IsOne() : other.IsOneRecursive();
  }
  if (IsLeaf() || n.IsLeaf()) {
    const auto &other = IsLeaf() ? n : *this;
    if (other.IsLeaf()) {
      return leaf_ == n.leaf_;
    }
    if (!fast && (other.IsZeroRecursive() || other.IsOneRecursive())) {
      const auto &leafy = IsLeaf() ? *this : n;
      VLOG(2) << "Assuming that leaf " << leafy.leaf_ << " not in {Zero, One}";
      return false;
    }
  }
  VLOG(0) << "Cannot compare general symbolic expressions "
          << *this << " vs. " << n << "; assuming they are not equal";
  return false;
}

template <class L>
void ExpressionNode<L>::Plus(
    std::shared_ptr<const ExpressionNode<L>> &n1,  // NB: non-const ref
    const std::shared_ptr<const ExpressionNode<L>> &n2) {
  if (n1->NotMember() || n2->IsZero()) {
    return;
  }
  if (n2->NotMember() || n1->IsZero()) {
    n1 = n2;
    return;
  }
  // Try to simplify (One + w w*) to w*.
  if (n1->IsOne() ^ n2->IsOne()) {
    const auto t = n1->IsOne() ? n2 : n1;
    // Special case: (One + One*) == (One + One One*) == One*.
    if (t->IsStar() && t->child1_->IsOne()) {
      n1 = t;
      return;
    }
    // General case: (One + w w*) == w*.
    if (t->IsTimes() && (t->child1_->IsStar() || t->child2_->IsStar())) {
      if (t->child1_->IsStar() &&
          t->child1_->child1_->IsEqual(*t->child2_, true)) {
        n1 = t->child1_;
        return;
      }
      if (t->child2_->IsStar() &&
          t->child2_->child1_->IsEqual(*t->child1_, true)) {
        n1 = t->child2_;
        return;
      }
    }
  }
  auto plus = std::make_shared<ExpressionNode<L>>(Expression::kPlus);
  plus->child1_ = std::move(n1);
  plus->child2_ = n2;
  n1 = std::move(plus);
}

template <class L>
void ExpressionNode<L>::Minus(
    std::shared_ptr<const ExpressionNode<L>> &n1,  // NB: non-const ref
    const std::shared_ptr<const ExpressionNode<L>> &n2) {
  if (n1->NotMember() || n2->IsZero()) {
    return;
  }
  if (n2->NotMember()) {
    n1 = n2;
    return;
  }
  if (n1->IsEqual(*n2, true)) {
    n1 = kZeroNode;
    return;
  }
  auto minus = std::make_shared<ExpressionNode<L>>(Expression::kMinus);
  minus->child1_ = std::move(n1);
  minus->child2_ = n2;
  n1 = std::move(minus);
}

template <class L>
void ExpressionNode<L>::Times(
    std::shared_ptr<const ExpressionNode<L>> &n1,  // NB: non-const ref
    const std::shared_ptr<const ExpressionNode<L>> &n2) {
  if (n1->NotMember() || n2->IsOne()) {
    return;
  }
  if (n2->NotMember() || n1->IsOne() || n2->IsZero()) {
    n1 = n2;
    return;
  }
  // The case of n1 == Zero could not have been handled earlier, since it was
  // not known whether n2->NotMember() is true prior to this point.
  if (n1->IsZero()) {
    return;
  }
  auto times = std::make_shared<ExpressionNode<L>>(Expression::kTimes);
  times->child1_ = std::move(n1);
  times->child2_ = n2;
  n1 = std::move(times);
}

template <class L>
void ExpressionNode<L>::Star(
    std::shared_ptr<const ExpressionNode<L>> &n) {  // NB: non-const ref
  if (n->NotMember()) {
    return;
  }
  if (n->IsZero()) {
    n = kOneNode;
    return;
  }
  auto star = std::make_shared<ExpressionNode<L>>(Expression::kStar);
  star->child1_ = std::move(n);
  n = std::move(star);
}

template <class L>
std::shared_ptr<const ExpressionNode<L>> ExpressionNode<L>::Reverse(
    const std::shared_ptr<const ExpressionNode<L>> &node) {
  std::shared_ptr<const ExpressionNode<L>> rev;
  switch (node->xcase_) {
    case Expression::EXPRESSION_NOT_SET:
    case Expression::kZero:
    case Expression::kOne:
    case Expression::kLeaf:
      rev = node;
      break;
    case Expression::kPlus:
    case Expression::kMinus:
    case Expression::kTimes:
    case Expression::kStar: {
      auto n = std::make_shared<ExpressionNode<L>>(node->xcase_);
      if (node->child1_) n->child1_ = ExpressionNode<L>::Reverse(node->child1_);
      if (node->child2_) n->child2_ = ExpressionNode<L>::Reverse(node->child2_);
      if (node->IsTimes()) {
        using std::swap;
        swap(n->child1_, n->child2_);
      }
      rev = std::move(n);
      break;
    }
    default:
      LOG(ERROR) << "expression case " << node->xcase_ << " not handled by "
                 << "Reverse()";
  }
  return rev;
}

template <class L>
class ExpressionNode<L>::Visitor {
 public:
  virtual ~Visitor() = default;

  virtual void VisitNoWeight() {}
  virtual void VisitZero() {}
  virtual void VisitOne() {}
  virtual void VisitLeaf(const typename L::Leaf &value) {}
  virtual void VisitPlus(const ExpressionNode<L> &child1,
                         const ExpressionNode<L> &child2) {}
  virtual void VisitMinus(const ExpressionNode<L> &child1,
                          const ExpressionNode<L> &child2) {}
  virtual void VisitTimes(const ExpressionNode<L> &child1,
                          const ExpressionNode<L> &child2) {}
  virtual void VisitStar(const ExpressionNode<L> &child) {}
};

template <class L>
void ExpressionNode<L>::Accept(Visitor *visitor) const {
  switch (xcase_) {
    case Expression::EXPRESSION_NOT_SET:
      visitor->VisitNoWeight();
      return;
    case Expression::kZero:
      visitor->VisitZero();
      return;
    case Expression::kOne:
      visitor->VisitOne();
      return;
    case Expression::kLeaf:
      visitor->VisitLeaf(leaf_);
      return;
    case Expression::kPlus:
      visitor->VisitPlus(*child1_, *child2_);
      return;
    case Expression::kMinus:
      visitor->VisitMinus(*child1_, *child2_);
      return;
    case Expression::kTimes:
      visitor->VisitTimes(*child1_, *child2_);
      return;
    case Expression::kStar:
      visitor->VisitStar(*child1_);
      return;
    default:
      LOG(ERROR) << "expression case " << xcase_ << " not handled by "
                 << "ExpressionNode Visitor";
  }
}

template <class L>
class ExpressionNodePrintVisitor : public ExpressionNode<L>::Visitor {
 public:
  ExpressionNodePrintVisitor(std::ostream &strm) : strm_(strm) {}

  void VisitNoWeight() override { strm_ << "NoWeight"; }

  void VisitZero() override { strm_ << "Zero"; }

  void VisitOne() override { strm_ << "One"; }

  void VisitLeaf(const typename L::Leaf &value) override { strm_ << value; }

  void VisitPlus(const ExpressionNode<L> &child1,
                 const ExpressionNode<L> &child2) override {
    strm_ << "(";
    child1.Accept(this);
    strm_ << " + ";
    child2.Accept(this);
    strm_ << ")";
  }

  void VisitMinus(const ExpressionNode<L> &child1,
                  const ExpressionNode<L> &child2) override {
    strm_ << "(";
    child1.Accept(this);
    strm_ << " - ";
    child2.Accept(this);
    strm_ << ")";
  }

  void VisitTimes(const ExpressionNode<L> &child1,
                  const ExpressionNode<L> &child2) override {
    strm_ << "[";
    child1.Accept(this);
    strm_ << " ";
    child2.Accept(this);
    strm_ << "]";
  }

  void VisitStar(const ExpressionNode<L> &child) override {
    child.Accept(this);
    strm_ << "*";
  }

 protected:
  std::ostream &strm_;
};

template <class L>
std::ostream &operator<<(std::ostream &strm, const ExpressionNode<L> &n) {
  ExpressionNodePrintVisitor<L> visitor(strm);
  n.Accept(&visitor);
  return strm;
}

}  // namespace festus

#endif  // FESTUS_EXPRESSION_NODE_H__
