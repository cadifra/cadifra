/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.Global;

import d1.wintypes;


namespace WinUtil
{

export class GlobalOwner
{
    d1::HGLOBAL glob_ = {}; // ownership

    static d1::HGLOBAL copy(d1::HGLOBAL g);

public:
    GlobalOwner() {}

    explicit GlobalOwner(d1::HGLOBAL g):
        glob_{ g } {}
    // Takes ownership of g

    GlobalOwner(const GlobalOwner& g):
        glob_{ copy(g.glob_) } {}
    // Copies g

    ~GlobalOwner() { reset(); }

    GlobalOwner& operator=(const GlobalOwner& g);
    // Frees glob_ and copies g

    GlobalOwner& operator=(d1::HGLOBAL g);
    // Frees glob_ and takes ownership of g

    d1::HGLOBAL get() const { return glob_; }

    d1::HGLOBAL release();
    // Releases ownership and sets glob_ = 0;

    void reset();
    // Frees glob_.
};


export template <typename T>
class GlobalLocker
{
    d1::HGLOBAL glob_;
    T* ptr_;

public:
    GlobalLocker(d1::HGLOBAL g):
        glob_{ g }
    {
        ptr_ = static_cast<T*>(::GlobalLock(glob_));
    }

    ~GlobalLocker()
    {
        ::GlobalUnlock(glob_);
    }

    operator T*() const { return ptr_; }

    T& operator*() const { return *ptr_; }

    T* operator->() const { return ptr_; }

    T* getPtr() const
    {
        return ptr_;
    }

    GlobalLocker(const GlobalLocker&) = delete;
    GlobalLocker& operator=(const GlobalLocker&) = delete;
};

}
