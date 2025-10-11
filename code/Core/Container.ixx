/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core:Container;

import :Interfaces;
import :IElement;


namespace Core
{

export class IContainer;
export class UpdateContainerParam;


export class IUpdateContainer: public virtual IShiftable
{
public:
    void UpdateContainer(Env&, UpdateContainerParam& p);

private:
    virtual void UpdateContainerImpl(Core::Env&, UpdateContainerParam& p) = 0;
};


export class UpdateContainerParam
{
    std::set<const IUpdateContainer*> itsSet;

public:
    UpdateContainerParam() {}
    UpdateContainerParam(const UpdateContainerParam&) = delete;
    UpdateContainerParam& operator=(const UpdateContainerParam&) = delete;

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
