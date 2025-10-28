module;

#include "d1assert.h"

export module d1.AutoComPtr;

#if 1 // copied from <winnt.h> and <minwindef.h>
      // (inclusion causes compilation problems)
#define STDMETHODCALLTYPE __stdcall
typedef unsigned long ULONG;
#endif

import std;


namespace d1
{

export template <class T>
class AutoComPtr
//
// Automatic Pointer to windows COM Object. Alternative to CcomPtr
// from <atlbase.h>.
//
// (Copied and adapted from class CComPtr from windows header <atlbase.h>)
//
// Use this template when you need the features of CComPtr but do not
// want to drag in the whole bunch of windows header files that <atlbase.h>
// includes (with ugly things as, for example, the macro "max" which may
// name-clash with std::numeric_limits<T>::max)
//
{
    T* p = nullptr;

public:
    AutoComPtr() {}

    AutoComPtr(T* lp)
    {
        if ((p = lp) != 0)
            p->AddRef();
    }

    AutoComPtr(const AutoComPtr& lp)
    {
        if ((p = lp.p) != 0)
            p->AddRef();
    }

    AutoComPtr(AutoComPtr&& lp) noexcept:
        p{ std::exchange(lp.p, nullptr) }
    {
    }

    ~AutoComPtr()
    {
        if (p)
            p->Release();
    }

    void Release()
    {
        if (p)
            p->Release();
        p = 0;
    }

    operator T*() const { return (T*)p; }

    T& operator*() const { return *p; }


    class Ptr
    {
        AutoComPtr* p;

    public:
        explicit Ptr(AutoComPtr* p):
            p{ p }
        {
            D1_ASSERT(p);
        }

        operator AutoComPtr*() { return p; }

        operator T**()
        {
            p->Release();
            return &(p->p);
        }

        operator void**()
        {
            p->Release();
            return reinterpret_cast<void**>(&(p->p));
        }
    };


    Ptr operator&() { return Ptr(this); }
    // only used when an aptr is used as a parameter on system call that
    // returns a new com object.

    class InaccessibleRelease: public T
    {
        virtual ULONG STDMETHODCALLTYPE Release() = 0;
    };

    auto* operator->() const { return static_cast<InaccessibleRelease*>(p); }
    // Warning: do *not* call aptr->Release(). call aptr.Release() instead.

    bool operator!() const { return (p == 0); }

    T* operator=(const AutoComPtr& lp)
    {
        if (lp.p)
            lp.p->AddRef();
        if (p)
            p->Release();
        p = lp.p;
        return p;
    }

    auto& operator=(AutoComPtr&& lp) noexcept
    {
        if (p == lp.p)
            return *this;

        if (T * old{ std::exchange(p, std::exchange(lp.p, nullptr)) })
            old->Release();

        return *this;
    }

    bool operator==(const AutoComPtr& lp) const
    {
        return p == lp.p;
    }

    bool operator==(const T* lp) const
    {
        return p == lp;
    }
};

}
