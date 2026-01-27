/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.CriticalSection;


/*******************************************************************************

  CriticalSection

When to use:

  You don't want that more than one thread enters a member function of the
  same object.
  You have this restriction for all member function of your class or only for
  some of them.

How to use:

  Add an additional attribute to your class:

    CriticalSection::Object CSO_;

  (You don't need to worry about it in your constructor/destructor or assignment
  operator.)

  In the member function you want to protect, add the following expression at
  the beginning of the function.

    CriticalSection criticalSection(CSO_);

  (the similarity with java is intended)

  (The same thread may recursively enter several times the scope of such a
   definition in any member function of the same object without being blocked.
   Only if a second thread wants to enter the scope of such a definition for an
   object X, the second thread has to wait until the first thread leaves the
   scope of all definitions in all member functions of the object X.)

/******************************************************************************/


namespace WinUtil
{

export class CriticalSection
{
public:
    class Object;

    explicit CriticalSection(Object& cso);
    ~CriticalSection();

    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;

private:
    Object& CSObject_;
};


class CriticalSection::Object
{
public:
    Object()
    {
        ::InitializeCriticalSection(&criticalSection_);
    }

    ~Object()
    {
        ::DeleteCriticalSection(&criticalSection_);
    }

    Object(const Object&)
    {
        ::InitializeCriticalSection(&criticalSection_);
        // the duplicate has nothing in common with the copy.
    }

    auto& operator=(const Object&)
    {
        /* nothing to do */
        return *this;
    }

private:
    friend class CriticalSection;
    CRITICAL_SECTION criticalSection_;
};


CriticalSection::CriticalSection(Object& cso):
    CSObject_{ cso }
{
    ::EnterCriticalSection(&CSObject_.criticalSection_);
}


CriticalSection::~CriticalSection()
{
    ::LeaveCriticalSection(&CSObject_.criticalSection_);
}

}
