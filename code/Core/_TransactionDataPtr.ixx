/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.TransactionDataPtr;

import std;

namespace Core
{

export template <class Data>
class TransactionDataPtr
{
    std::unique_ptr<Data> data_;

public:
    TransactionDataPtr()
    {
    }

    TransactionDataPtr(const TransactionDataPtr&):
        data_{} // Data is not copied!
    {
    }

    TransactionDataPtr& operator=(const TransactionDataPtr& rhs)
    {
        reset();  // rhs is not assigned!
    }

    void reset()
    {
        data_.reset();
    }

    void assign(std::unique_ptr<Data> d)
    {
        data_ = std::move(d);
    }

    Data* get() const { return data_.get(); }
    Data& operator*() const { return *data_; }
    operator bool() const { return data_.get() != nullptr; }

    Data* operator->() const { return data_.get(); }
};

}
