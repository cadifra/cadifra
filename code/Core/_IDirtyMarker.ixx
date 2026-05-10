/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.IDirtyMarker;

import d1.Observer;

namespace Core
{

export class IDirtyStateChange
{
public:
    virtual void dirtyStateChanged(bool IsDirty) = 0;

protected:
    ~IDirtyStateChange() = default;
};


export class IDirtyMarker
{
public:
    IDirtyMarker() = default;
    IDirtyMarker(const IDirtyMarker&) = delete;
    IDirtyMarker& operator=(const IDirtyMarker&) = delete;

    virtual ~IDirtyMarker() = default;

    virtual bool isDirty() const = 0;

    virtual void clearDirtyFlag(bool notifyObservers = true) = 0;

    virtual auto getDirtyStateConnector()
        -> d1::Observer::C<IDirtyStateChange> = 0;
};

}
