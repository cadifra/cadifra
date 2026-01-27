/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.RefCounter;

/*
The class RefCounter can be used to implement the two member functions
AddRef() and Release() of COM classes as in the following example:

  class IDoNothing: public IUnknown
  {
    d1::RefCounter<IDoNothing> refCounter_;

  public:

    virtual ULONG STDMETHODCALLTYPE AddRef()
    {
      return refCounter_.AddRef();
    }

    virtual ULONG STDMETHODCALLTYPE Release()
    {
      return refCounter_.Release(this);
    }

    ....

  };

*/

namespace d1
{

export template <class T>
class RefCounter
{
    unsigned long count_ = 0;

public:
    RefCounter() {}

    unsigned long AddRef()
    {
        return ++count_;
    }

    unsigned long Release(T* t)
    {
        unsigned long res = --count_;
        if (count_ == 0)
        {
            delete t;
        }
        return res;
    }
};

}
