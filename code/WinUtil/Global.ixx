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
    d1::HGLOBAL itsGlob = {}; // ownership

    static d1::HGLOBAL Copy(d1::HGLOBAL g);

public:
    GlobalOwner() {}

    explicit GlobalOwner(d1::HGLOBAL g):
        itsGlob{ g } {}
    // Takes ownership of g

    GlobalOwner(const GlobalOwner& g):
        itsGlob{ Copy(g.itsGlob) } {}
    // Copies g

    ~GlobalOwner() { Reset(); }

    GlobalOwner& operator=(const GlobalOwner& g);
    // Frees itsGlob and copies g

    GlobalOwner& operator=(d1::HGLOBAL g);
    // Frees itsGlob and takes ownership of g

    d1::HGLOBAL Get() const { return itsGlob; }

    d1::HGLOBAL Release();
    // Releases ownership and sets itsGlob = 0;

    void Reset();
    // Frees itsGlob.
};


export template <typename T>
class GlobalLocker
{
    d1::HGLOBAL itsGlob;
    T* itsPtr;

public:
    GlobalLocker(d1::HGLOBAL g):
        itsGlob{ g }
    {
        itsPtr = static_cast<T*>(::GlobalLock(itsGlob));
    }

    ~GlobalLocker()
    {
        ::GlobalUnlock(itsGlob);
    }

    operator T*() const { return itsPtr; }

    T& operator*() const { return *itsPtr; }

    T* operator->() const { return itsPtr; }

    T* GetPtr() const
    {
        return itsPtr;
    }

    GlobalLocker(const GlobalLocker&) = delete;
    GlobalLocker& operator=(const GlobalLocker&) = delete;
};

}
