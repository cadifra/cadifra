/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module App.NumberedWindow;

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

}
