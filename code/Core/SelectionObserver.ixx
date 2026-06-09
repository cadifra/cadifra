/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module Core.SelectionObserver;

import d1.algorithm;

import std;


namespace Core
{

export class ISelectionObserver
{
public:
    class Dock;

    ISelectionObserver() {}

    ISelectionObserver(const ISelectionObserver&) = delete;
    ISelectionObserver& operator=(const ISelectionObserver&) = delete;

    virtual ~ISelectionObserver();

    virtual void selectionChanged() = 0;
    // The observed selection may have been changed.

    void setDock(Dock* d) { dock_ = d; }

private:
    Dock* dock_ = nullptr;
};


class ISelectionObserver::Dock
{
    using SelObsContainer = std::vector<ISelectionObserver*>;
    SelObsContainer observers_;

public:
    Dock() {}
    Dock(const Dock&) = delete;
    Dock& operator=(const Dock&) = delete;

    void attach(ISelectionObserver& obs)
    {
        observers_.push_back(&obs);
        obs.setDock(this);
    }

    void detach(ISelectionObserver& obs)
    {
        d1::erase_first(observers_, &obs);
        obs.setDock(nullptr);
    }

    void notify() const
{
    for (auto* obs : observers_)
    {
        D1_ASSERT(obs);
        obs->selectionChanged();
    }
}

};


ISelectionObserver::~ISelectionObserver()
{
    if (dock_)
    {
        dock_->detach(*this);
        dock_ = nullptr;
    }
}

}
