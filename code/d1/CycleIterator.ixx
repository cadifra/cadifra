/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.CycleIterator;

import std;


namespace d1
{

export template <class Iter, class ValueType>
class CycleIterator
{
    Iter begin_;
    Iter end_;
    Iter current_;

public:
    using value_type = ValueType;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = ValueType*;
    using reference = ValueType&;

    CycleIterator():
        end_{ begin_ }, current_{ begin_ }
    {
    }


    explicit CycleIterator(
        const Iter& begin, const Iter& end):
        begin_{ begin },
        end_{ end }, current_{ begin }
    {
    }


    explicit CycleIterator(
        const Iter& begin, const Iter& end, const Iter& current):
        begin_{ begin },
        end_{ end }, current_{ current }
    {
    }


    CycleIterator(const CycleIterator&) = default;


    CycleIterator& operator=(const CycleIterator&) = default;


    value_type& operator*()
    {
        return *current_;
    }


    auto& operator++()
    {
        ++current_;
        if (current_ == end_)
            current_ = begin_;
        return *this;
    }

    /*
      CycleIterator& operator++(int) {
        CycleWriter temp = *this;
        ++current_;
        if(current_ == end_) current_ = begin_;
        return temp;
      }
    */
};


}
