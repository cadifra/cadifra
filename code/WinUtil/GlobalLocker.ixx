/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil:GlobalLocker;


namespace WinUtil
{

export template <typename T>
class GlobalLocker
{
    HGLOBAL itsGlob;
    T* itsPtr;

public:
    GlobalLocker(HGLOBAL g):
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
