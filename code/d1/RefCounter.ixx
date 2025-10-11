/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.RefCounter;

/*
The class RefCounter can be used to implement the two member functions
AddRef() and Release() of COM classes as in the following example:

  class IDoNothing: public IUnknown
  {
    d1::RefCounter<IDoNothing> itsRefCounter;

  public:

    virtual ULONG STDMETHODCALLTYPE AddRef()
    {
      return itsRefCounter.AddRef();
    }

    virtual ULONG STDMETHODCALLTYPE Release()
    {
      return itsRefCounter.Release(this);
    }

    ....

  };

*/

namespace d1
{

export template <class T>
class RefCounter
{
    unsigned long itsCount = 0;

public:
    RefCounter() {}

    unsigned long AddRef()
    {
        return ++itsCount;
    }

    unsigned long Release(T* t)
    {
        unsigned long res = --itsCount;
        if (itsCount == 0)
        {
            delete t;
        }
        return res;
    }
};

}
