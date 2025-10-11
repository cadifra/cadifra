/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil:CriticalSection;


/*******************************************************************************

  CriticalSection & CriticalSectionObject

When to use:

  You don't want that more than one thread enters a member function of the
  same object.
  You have this restriction for all member function of your class or only for
  some of them.

How to use:

  Add an additional attribute to your class:

    CriticalSectionObject itsCSO;

  (You don't need to worry about it in your constructor/destructor or assignment
  operator.)

  In the member function you want to protect, add the following expression at
  the beginning of the function.

    CriticalSection criticalSection(itsCSO);

  (the similarity with java is intended)

  (The same thread may recursively enter several times the scope of such a
   definition in any member function of the same object without being blocked.
   Only if a second thread wants to enter the scope of such a definition for an
   object X, the second thread has to wait until the first thread leaves the
   scope of all definitions in all member functions of the object X.)

/******************************************************************************/


namespace WinUtil
{

export class CriticalSection;

export class CriticalSectionObject
{
public:
    CriticalSectionObject()
    {
        ::InitializeCriticalSection(&itsCriticalSection);
    }

    ~CriticalSectionObject()
    {
        ::DeleteCriticalSection(&itsCriticalSection);
    }

    CriticalSectionObject(const CriticalSectionObject&)
    {
        ::InitializeCriticalSection(&itsCriticalSection);
        // the duplicate has nothing in common with the copy.
    }

    auto& operator=(const CriticalSectionObject&)
    {
        /* nothing to do */
        return *this;
    }

private:
    friend class CriticalSection;
    CRITICAL_SECTION itsCriticalSection;
};


export class CriticalSection
{
public:
    explicit CriticalSection(CriticalSectionObject& cso):
        itsCSObject{ cso }
    {
        ::EnterCriticalSection(&itsCSObject.itsCriticalSection);
    }

    ~CriticalSection()
    {
        ::LeaveCriticalSection(&itsCSObject.itsCriticalSection);
    }

    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;

private:
    CriticalSectionObject& itsCSObject;
};

}
