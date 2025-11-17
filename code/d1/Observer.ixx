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
    List<O>* itsList = nullptr;
    O* itsObserver = nullptr;

public:
    Connector(List<O>* = 0);
    ~Connector(); // intentionally NOT virtual

    Connector(const Connector&);
    auto operator=(const Connector&) -> Connector&;

    void Connect(O&);
    void Disconnect();
};

export template <class O>
using C = Connector<O>;


export template <class O>
class List
{
    std::vector<O*> itsList; // ref only pointers

public:
    void Add(O& obs) 
    // Adds obs to its internal list. Don't use during a "Notify"-call!
    {
        auto i = std::ranges::find(itsList, nullptr);

        if (i != end(itsList))
            *i = &obs;
        else
            itsList.push_back(&obs);
    }

    void Forget(O& obs)
    // Removes the first occurrence of obs from its internal list.
    {
        auto i = std::ranges::find(itsList, &obs);

        if (i != end(itsList))
            *i = nullptr;
    }

    auto GetConnector()
    {
        return Connector<O>(this);
    }

    void Notify(const auto& function)
    {
        for (auto i = begin(itsList); i != end(itsList); /* no ++i*/)
        {
            if (*i != nullptr)
                function(*i++);
            else
                i = itsList.erase(i);
        }
    }
};

export template <class O>
using L = List<O>;


template <class O>
inline Connector<O>::Connector(List<O>* list):
    itsList{ list }
{
}


template <class O>
inline Connector<O>::~Connector()
{
    Disconnect();
}


template <class O>
inline void Connector<O>::Connect(O& obs)
{
    D1_ASSERT(itsObserver == 0);
    D1_ASSERT(itsList);
    itsList->Add(obs);
    itsObserver = &obs;
}


template <class O>
inline void Connector<O>::Disconnect()
{
    if (itsObserver && itsList)
        itsList->Forget(*itsObserver);
    itsObserver = 0;
}


template <class O>
inline Connector<O>::Connector(const Connector& rhs):
    itsList{ rhs.itsList },
    itsObserver{}
{
}


template <class O>
inline auto Connector<O>::operator=(const Connector& rhs) -> Connector&
{
    if (this != &rhs)
    {
        Disconnect();
        itsList = rhs.itsList;
        itsObserver = 0;
    }
    return *this;
}

}
