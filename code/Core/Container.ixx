/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Container;

import Core.Interfaces;
import Core.Shift;

import Core.Main;

namespace Core
{

export class IContainer;


export class IUpdateContainer: public virtual IShiftable
{
public:
    class Param;

    void update(Env&, Param& p);

private:
    virtual void updateImpl(Env&, Param& p) = 0;
};


class IUpdateContainer::Param
{
    std::set<const IUpdateContainer*> set_;

public:
    Param() {}
    Param(const Param&) = delete;
    Param& operator=(const Param&) = delete;

    bool add(const IUpdateContainer* c) { return (set_.insert(c)).second; }
};


void IUpdateContainer::update(Env& e, Param& p)
{
    if (p.add(this))
        updateImpl(e, p);
}


export class IContainable: public IUpdateContainer, public IDeletable
{
public:
    virtual void setContainer(Env&, IContainer*) = 0;
    virtual auto getContainer() const -> IContainer* = 0;
};


export class IContainer: public virtual IElement
{
public:
    virtual void add(Env&, IContainable&) = 0;
    virtual void forget(Env&, const IContainable&) = 0;
    virtual bool has(const IContainable&) const = 0;
    virtual void initialAdd(Env&, IContainable&) = 0;
};

}
