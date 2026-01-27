/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1assert.h"

export module d1.Observer;

import std;


namespace d1::Observer
{

export template <class O>
class List;


export template <class O>
class Connector
{
    List<O>* list_ = nullptr;
    O* observer_ = nullptr;

public:
    Connector(List<O>* = 0);
    ~Connector(); // intentionally NOT virtual

    Connector(const Connector&);
    auto operator=(const Connector&) -> Connector&;

    void connect(O&);
    void disconnect();
};

export template <class O>
using C = Connector<O>;


export template <class O>
class List
{
    std::vector<O*> list_; // ref only pointers

public:
    void add(O& obs) 
    // Adds obs to its internal list. Don't use during a "Notify"-call!
    {
        auto i = std::ranges::find(list_, nullptr);

        if (i != end(list_))
            *i = &obs;
        else
            list_.push_back(&obs);
    }

    void forget(O& obs)
    // Removes the first occurrence of obs from its internal list.
    {
        auto i = std::ranges::find(list_, &obs);

        if (i != end(list_))
            *i = nullptr;
    }

    auto getConnector()
    {
        return Connector<O>(this);
    }

    void notify(const auto& function)
    {
        for (auto i = begin(list_); i != end(list_); /* no ++i*/)
        {
            if (*i != nullptr)
                function(*i++);
            else
                i = list_.erase(i);
        }
    }
};

export template <class O>
using L = List<O>;


template <class O>
inline Connector<O>::Connector(List<O>* list):
    list_{ list }
{
}


template <class O>
inline Connector<O>::~Connector()
{
    disconnect();
}


template <class O>
inline void Connector<O>::connect(O& obs)
{
    D1_ASSERT(observer_ == 0);
    D1_ASSERT(list_);
    list_->add(obs);
    observer_ = &obs;
}


template <class O>
inline void Connector<O>::disconnect()
{
    if (observer_ and list_)
        list_->forget(*observer_);
    observer_ = 0;
}


template <class O>
inline Connector<O>::Connector(const Connector& rhs):
    list_{ rhs.list_ },
    observer_{}
{
}


template <class O>
inline auto Connector<O>::operator=(const Connector& rhs) -> Connector&
{
    if (this != &rhs)
    {
        disconnect();
        list_ = rhs.list_;
        observer_ = 0;
    }
    return *this;
}

}
