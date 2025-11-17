/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core:Container;

import :Base;
import :Interfaces;


namespace Core
{

export class IContainer;


export class IUpdateContainer: public virtual IShiftable
{
public:
    class Param;

    void UpdateContainer(Env&, Param& p);

private:
    virtual void UpdateContainerImpl(Env&, Param& p) = 0;
};


class IUpdateContainer::Param
{
    std::set<const IUpdateContainer*> itsSet;

public:
    Param() {}
    Param(const Param&) = delete;
    Param& operator=(const Param&) = delete;

    bool Add(const IUpdateContainer* c) { return (itsSet.insert(c)).second; }
};


export class IContainable: public IUpdateContainer, public IDeletable
{
public:
    virtual void SetContainer(Env&, IContainer*) = 0;
    virtual auto GetContainer() const -> IContainer* = 0;
};


export class IContainer: public virtual IElement
{
public:
    virtual void Add(Env&, IContainable&) = 0;
    virtual void Forget(Env&, const IContainable&) = 0;
    virtual bool Has(const IContainable&) const = 0;
    virtual void InitialAdd(Env&, IContainable&) = 0;
};

}
