// festus/iterator.h
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
// Façades for iterating over FST states and arcs with C++11 range-based loops.
//
// OpenFst's StateIterator and ArcIterator templates do not work directly with
// C++11's range-based for-loop construct. Instead the typical usage idiom is:
//
// for (StateIterator<FooFst<BarArc>> siter(fst); !siter.Done(); siter.Next()) {
//   auto state = siter.Value();
//   for (ArcIterator<FooFst<BarArc>> aiter(fst, state); !aiter.Done();
//        aiter.Next()) {
//     const auto &arc = aiter.Value();
//     // do stuff
//   }
// }
//
// This tends to get verbose. Note that there is no obvious way to allow for
// automatic type inference of the iterator types. They have to be declared
// explicitly. Further, writing StateIterator<FooFst<BarArc>> is usually
// preferred over writing e.g. StateIterator<Fst<BarArc>>, since the template
// specialization of StateIterator<FooFst<...>> is often more efficient than the
// instantiation as the basic StateIterator<Fst<...>>.
//
// The façades below partly address this situation. The typical usage is now:
//
// for (auto state : festus::States(fst)) {
//   for (const auto &arc : festus::Arcs(fst, state)) {
//     // do stuff
//   }
// }
//
// This allows for the automatic inference of template argument types in the
// instantiation of the template functions States<> and Arcs<>, which will match
// the declared type of 'fst'.

#ifndef FESTUS_ITERATOR_H__
#define FESTUS_ITERATOR_H__

#include <cstddef>
#include <iterator>
#include <memory>

#include <fst/compat.h>
#include <fst/fst.h>

namespace festus {

template <class F>
class StateFacade {
 public:
  typedef typename F::StateId StateId;

  // Wraps a StateIterator and its current value.
  class const_iterator {
   public:
    typedef std::ptrdiff_t difference_type;
    typedef StateId value_type;
    typedef const StateId *pointer;
    typedef const StateId &reference;
    typedef std::input_iterator_tag iterator_category;

    const_iterator() = default;

    const_iterator(const_iterator &&) = default;
    const_iterator &operator=(const_iterator &&) = default;

    const_iterator(const const_iterator &that) = default;
    const_iterator &operator=(const const_iterator &that) = default;

    const_iterator(const F &fst)
        : iterator_(new fst::StateIterator<F>(fst)),
          value_(iterator_->Done() ? fst::kNoStateId : iterator_->Value()) {
    }

    const_iterator(StateId value) : value_(value) {}

    StateId operator*() const { return value_; }

    const_iterator &operator++() {
      if (iterator_ && !iterator_->Done()) {
        iterator_->Next();
        value_ = iterator_->Done() ? fst::kNoStateId : iterator_->Value();
      }
      return *this;
    }

    const_iterator operator++(int) {
      const_iterator result(value_);
      operator++();
      return result;
    }

    friend bool operator==(const const_iterator &lhs,
                           const const_iterator &rhs) {
      return lhs.value_ == rhs.value_;
    }

    friend bool operator!=(const const_iterator &lhs,
                           const const_iterator &rhs) {
      return !(lhs == rhs);
    }

   private:
    std::shared_ptr<fst::StateIterator<F>> iterator_;
    StateId value_ = fst::kNoStateId;
  };

  StateFacade() = delete;

  // Does not copy 'fst'. The lifespan of 'fst' must match or exceed the
  // lifespan of this object.
  explicit StateFacade(const F &fst) : fst_(fst) {}

  const_iterator begin() const { return const_iterator(fst_); }

  const_iterator end() const { return const_iterator(); }

 private:
  const F &fst_;
};

template <class F>
inline StateFacade<F> States(const F &fst) {
  return StateFacade<F>(fst);
}

////////////////////////////////////////////////////////////////////////////////

template <class F>
class ArcFacade {
 public:
  typedef typename F::Arc Arc;
  typedef typename F::StateId StateId;

  // Wraps an ArcIterator and its current value.
  class const_iterator {
   public:
    typedef std::ptrdiff_t difference_type;
    typedef Arc value_type;
    typedef const Arc *pointer;
    typedef const Arc &reference;
    typedef std::input_iterator_tag iterator_category;

    const_iterator() = default;

    const_iterator(const_iterator &&) = default;
    const_iterator &operator=(const_iterator &&) = default;

    const_iterator(const const_iterator &) = default;
    const_iterator &operator=(const const_iterator &) = default;

    const_iterator(const F &fst, StateId state)
        : iterator_(new fst::ArcIterator<F>(fst, state)),
          value_(iterator_->Done() ? nullptr : &iterator_->Value()) {
    }

    const_iterator(const Arc *value) : value_(value) {}

    const Arc &operator*() const { return *value_; }

    const_iterator &operator++() {
      if (iterator_ && !iterator_->Done()) {
        iterator_->Next();
        value_ = iterator_->Done() ? nullptr : &iterator_->Value();
      }
      return *this;
    }

    const_iterator operator++(int) {
      const_iterator result(value_);
      operator++();
      return result;
    }

    friend bool operator==(const const_iterator &lhs,
                           const const_iterator &rhs) {
      if (lhs.iterator_ && rhs.iterator_) {
        DCHECK_EQ(lhs.iterator_.get(), rhs.iterator_.get());
        return lhs.iterator_->Position() == rhs.iterator_->Position();
      } else {
        return lhs.value_ == rhs.value_;
      }
    }

    friend bool operator!=(const const_iterator &lhs,
                           const const_iterator &rhs) {
      return !(lhs == rhs);
    }

   private:
    std::shared_ptr<fst::ArcIterator<F>> iterator_;
    const Arc *value_ = nullptr;
  };

  ArcFacade() = delete;

  // Does not copy 'fst'. The lifespan of 'fst' must match or exceed the
  // lifespan of this object.
  ArcFacade(const F &fst, StateId state) : fst_(fst), state_(state) {}

  const_iterator begin() const { return const_iterator(fst_, state_); }

  const_iterator end() const { return const_iterator(); }

 private:
  const F &fst_;
  StateId state_;
};

template <class F>
inline ArcFacade<F> Arcs(const F &fst, typename F::StateId state) {
  return ArcFacade<F>(fst, state);
}

}  // namespace festus

#endif  // FESTUS_ITERATOR_H__
