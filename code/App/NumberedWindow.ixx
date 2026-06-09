/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module App.NumberedWindow;

import d1.algorithm;

import std;


namespace App
{

export class INumberedWindow
{
public:
    class Numberer;

    virtual void setWindowNumber(int number) = 0;

protected:
    ~INumberedWindow() = default;
};


// A Numberer allocates numbers to the registered INumberedWindow
// objects.
// It calls the member function SetWindowNumber of every registered
// INumberedWindow if a new INumberedWindow is registered or an old one
// is unregistered.
//
// The allocated numbers start with the number 1 if more than one
// INumberedWindow is registered; it starts with 0 if only one
// INumberedWindow exists.

class INumberedWindow::Numberer
{
public:
    Numberer() {}
    Numberer(const Numberer&) = delete;
    Numberer& operator=(const Numberer&) = delete;

    void add(INumberedWindow& window);
    // register a new INumberedWindow

    void forget(INumberedWindow& window);
    // unregister a INumberedWindow

private:
    using ItsWindowsType = std::vector<INumberedWindow*>; // ref only ptrs
    ItsWindowsType windows_;
    void allocateNumbers();
};



void INumberedWindow::Numberer::add(INumberedWindow& w)
{
    D1_ASSERT(end(windows_) == std::ranges::find(windows_, &w));

    windows_.push_back(&w);
    allocateNumbers();
}


void INumberedWindow::Numberer::forget(INumberedWindow& w)
{
    D1_ASSERT(
        1 == std::count(begin(windows_), end(windows_), &w));

    d1::erase_first(windows_, &w);
    allocateNumbers();
}


void INumberedWindow::Numberer::allocateNumbers()
{
    switch (windows_.size())
    {
    case 1:
        (*begin(windows_))->setWindowNumber(0);
        break;

    case 0:
        // do nothing
        break;

    default:
        for (int n = 1; auto* w : windows_)
            w->setWindowNumber(n++);
    }
}

}
