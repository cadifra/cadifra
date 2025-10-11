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
class IPlug
{
public:
    virtual void Add(O&) = 0;
    virtual void Forget(O&) = 0;

protected:
    ~IPlug() = default;
};


export template <class O>
class Connector
{
    IPlug<O>* itsPlug = nullptr;
    O* itsObserver = nullptr;

public:
    Connector(IPlug<O>* = 0);
    ~Connector(); // intentionally NOT virtual

    Connector(const Connector&);
    auto operator=(const Connector&) -> Connector&;

    void Connect(O&);
    void Disconnect();
};

export template <class O>
using C = Connector<O>;


export template <class O>
class List:
    private IPlug<O>
{
    std::vector<O*> itsList; // ref only pointers

public:
    //-- IPlug

    void Add(O& obs) override
    // Adds obs to its internal list. Don't use during a "Notify"-call!
    {
        auto i = std::ranges::find(itsList, nullptr);

        if (i != end(itsList))
            *i = &obs;
        else
            itsList.push_back(&obs);
    }

    void Forget(O& obs) override
    // Removes the first occurrence of obs from its internal list.
    {
        auto i = std::ranges::find(itsList, &obs);

        if (i != end(itsList))
            *i = nullptr;
    }

    //--

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
inline Connector<O>::Connector(IPlug<O>* plug):
    itsPlug{ plug }
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
    D1_ASSERT(itsPlug);
    itsPlug->Add(obs);
    itsObserver = &obs;
}


template <class O>
inline void Connector<O>::Disconnect()
{
    if (itsObserver && itsPlug)
        itsPlug->Forget(*itsObserver);
    itsObserver = 0;
}


template <class O>
inline Connector<O>::Connector(const Connector& rhs):
    itsPlug{ rhs.itsPlug },
    itsObserver{}
{
}


template <class O>
inline auto Connector<O>::operator=(const Connector& rhs) -> Connector&
{
    if (this != &rhs)
    {
        Disconnect();
        itsPlug = rhs.itsPlug;
        itsObserver = 0;
    }
    return *this;
}

}
