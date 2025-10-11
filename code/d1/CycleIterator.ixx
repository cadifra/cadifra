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
    Iter itsBegin;
    Iter itsEnd;
    Iter itsCurrent;

public:
    using value_type = ValueType;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = ValueType*;
    using reference = ValueType&;

    CycleIterator():
        itsEnd{ itsBegin }, itsCurrent{ itsBegin }
    {
    }


    explicit CycleIterator(
        const Iter& begin, const Iter& end):
        itsBegin{ begin },
        itsEnd{ end }, itsCurrent{ begin }
    {
    }


    explicit CycleIterator(
        const Iter& begin, const Iter& end, const Iter& current):
        itsBegin{ begin },
        itsEnd{ end }, itsCurrent{ current }
    {
    }


    CycleIterator(const CycleIterator&) = default;


    CycleIterator& operator=(const CycleIterator&) = default;


    value_type& operator*()
    {
        return *itsCurrent;
    }


    auto& operator++()
    {
        ++itsCurrent;
        if (itsCurrent == itsEnd)
            itsCurrent = itsBegin;
        return *this;
    }

    /*
      CycleIterator& operator++(int) {
        CycleWriter temp = *this;
        ++itsCurrent;
        if(itsCurrent == itsEnd) itsCurrent = itsBegin;
        return temp;
      }
    */
};


}
