/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

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

module : private;


namespace WinUtil
{

HGLOBAL GlobalOwner::copy(HGLOBAL g)
{
    if (not g)
        return 0;

    DWORD size = static_cast<DWORD>(::GlobalSize(g));
    D1_ASSERT(size);

    HGLOBAL res = ::GlobalAlloc(GMEM_MOVEABLE, size);
    D1_ASSERT(res);

    void* dest = ::GlobalLock(res);
    D1_ASSERT(dest);

    void* source = ::GlobalLock(g);
    D1_ASSERT(source);

    ::CopyMemory(dest, source, size);

    ::GlobalUnlock(dest);
    ::GlobalUnlock(source);

    return res;
}


GlobalOwner& GlobalOwner::operator=(const GlobalOwner& g)
{
    if (glob_ == g.glob_)
        return *this;
    reset();
    glob_ = copy(g.glob_);
    return *this;
}


GlobalOwner& GlobalOwner::operator=(HGLOBAL g)
{
    if (glob_ == g)
        return *this;
    reset();
    glob_ = g;
    return *this;
}


HGLOBAL GlobalOwner::release()
{
    HGLOBAL t = glob_;
    glob_ = 0;
    return t;
}


void GlobalOwner::reset()
{
    if (glob_)
        glob_ = ::GlobalFree(glob_);
    D1_ASSERT(not glob_);
}

}
