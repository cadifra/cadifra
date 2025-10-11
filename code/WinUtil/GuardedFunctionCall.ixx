/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil:GuardedFunctionCall;

import :ComException;

import std;


namespace WinUtil
{

export class GuardedFunctionCall
{
    const char* itsCaller;

public:
    enum class Result
    {
        ok = 0,
        exception_catched
    };

    GuardedFunctionCall(const char* caller) throw():
        itsCaller{ caller }
    {
    }
    virtual ~GuardedFunctionCall() throw() = default;

    virtual Result Execute() throw(); // calls this->ImplementCall()

private:
    virtual void ImplementCall() = 0; /* may throw any exception */
    Result CallL2();
    Result CallL3();

    void ShowErrorBox(const std::exception& e) const;
};


export template <class T>
class GuardedFunctionCallRet: public GuardedFunctionCall
{
    T itsRes;
    T itsFailureRes;

public:
    GuardedFunctionCallRet(const char* caller, T failure_res) throw():
        GuardedFunctionCall{ caller }, itsRes{ failure_res }, itsFailureRes{ failure_res }
    {
    }
    Result Execute() throw() override
    {
        Result r = GuardedFunctionCall::Execute();
        if (r != Result::ok)
            itsRes = itsFailureRes; // exception occurred
        return r;
    }
    T Res() const throw() { return itsRes; }

private:
    void ImplementCall() override
    {
        itsRes = ImplementCallRet(); /* may throw any exception */
    }
    virtual T ImplementCallRet() = 0;
};


namespace GuardedComFunctionCall
{

// helper class:

template <class R>
class GFC: public GuardedFunctionCallRet<R>
{
    virtual R Call() = 0;
    R ImplementCallRet() { return Call(); }

public:
    GFC(const char* s, R e):
        GuardedFunctionCallRet<R>{ s, e }
    {
    }
    R operator()()
    {
        this->Execute();
        return this->Res();
    }
};


template <>
class GFC<HRESULT>: public GuardedFunctionCallRet<HRESULT>
{
    virtual HRESULT Call() = 0;
    HRESULT ImplementCallRet()
    {
        try
        {
            return Call();
        }
        catch (ComException& e)
        {
            return e.GetHRESULT();
        }
    }

public:
    GFC(const char* s, HRESULT e):
        GuardedFunctionCallRet<HRESULT>{ s, e }
    {
    }
    HRESULT operator()()
    {
        this->Execute();
        return this->Res();
    }
};


/*
  Template parameters:

  T      = target type (IUnknown, IDataObject etc.)
  R      = result type (HRESULT etc.)
  P1..P? = parameter type
  PMF    = pointer to member function type (&IUnknown::AddRef etc.)

  Usage example (IUnknown::QueryInterface):

    return GFC_2P<IUnknown, HRESULT, REFIID, void**>("QueryInterface",
      target, riid, ppvObject, E_UNEXPECTED, &IUnknown::QueryInterface)();

  is the same as:

    try {
      return target.QueryInterface(riid, ppvObject);
    }
    catch(WinUtil::ComException& e) {
      return e.GetHRESULT();
    }
    catch(...) {
      heavy stuff here...
      return E_UNEXPECTED;
    }
*/


template <class T, class R>
class GFC_0P: public GFC<R>
{
    using PMF = R (STDMETHODCALLTYPE T::*)();
    T& t;
    PMF pmf;

public:
    GFC_0P(const char* s, T& _t, R e, PMF _pmf):
        GFC<R>{ s, e }, t{ _t }, pmf{ _pmf }
    {
    }
    R Call() { return (t.*pmf)(); }
};


template <class T, class R, class P1>
class GFC_1P: public GFC<R>
{
    using PMF = R (STDMETHODCALLTYPE T::*)(P1);
    T& t;
    P1 p1;
    PMF pmf;

public:
    GFC_1P(const char* s, T& _t, P1 _p1, R e, PMF _pmf):
        GFC<R>{ s, e }, t{ _t }, p1{ _p1 }, pmf{ _pmf }
    {
    }
    R Call() { return (t.*pmf)(p1); }
};


template <class T, class R, class P1, class P2>
class GFC_2P: public GFC<R>
{
    using PMF = R (STDMETHODCALLTYPE T::*)(P1, P2);
    T& t;
    P1 p1;
    P2 p2;
    PMF pmf;

public:
    GFC_2P(const char* s, T& _t, P1 _p1, P2 _p2, R e, PMF _pmf):
        GFC<R>{ s, e }, t{ _t }, p1{ _p1 }, p2{ _p2 }, pmf{ _pmf }
    {
    }
    R Call() { return (t.*pmf)(p1, p2); }
};


template <class T, class R, class P1, class P2, class P3>
class GFC_3P: public GFC<R>
{
    using PMF = R (STDMETHODCALLTYPE T::*)(P1, P2, P3);
    T& t;
    P1 p1;
    P2 p2;
    P3 p3;
    PMF pmf;

public:
    GFC_3P(const char* s, T& _t, P1 _p1, P2 _p2, P3 _p3, R e, PMF _pmf):
        GFC<R>{ s, e }, t{ _t }, p1{ _p1 }, p2{ _p2 }, p3{ _p3 }, pmf{ _pmf }
    {
    }
    R Call() { return (t.*pmf)(p1, p2, p3); }
};


template <class T, class R, class P1, class P2, class P3, class P4>
class GFC_4P: public GFC<R>
{
    using PMF = R (STDMETHODCALLTYPE T::*)(P1, P2, P3, P4);
    T& t;
    P1 p1;
    P2 p2;
    P3 p3;
    P4 p4;
    PMF pmf;

public:
    GFC_4P(const char* s, T& _t, P1 _p1, P2 _p2, P3 _p3, P4 _p4, R e, PMF _pmf):
        GFC<R>{ s, e }, t{ _t }, p1{ _p1 }, p2{ _p2 }, p3{ _p3 }, p4{ _p4 }, pmf{ _pmf }
    {
    }
    R Call() { return (t.*pmf)(p1, p2, p3, p4); }
};


template <class T, class R, class P1, class P2, class P3, class P4, class P5>
class GFC_5P: public GFC<R>
{
    using PMF = R (STDMETHODCALLTYPE T::*)(P1, P2, P3, P4, P5);
    T& t;
    P1 p1;
    P2 p2;
    P3 p3;
    P4 p4;
    P5 p5;
    PMF pmf;

public:
    GFC_5P(const char* s, T& _t, P1 _p1, P2 _p2, P3 _p3, P4 _p4, P5 _p5, R e, PMF _pmf):
        GFC<R>{ s, e }, t{ _t }, p1{ _p1 }, p2{ _p2 }, p3{ _p3 },
        p4{ _p4 }, p5{ _p5 }, pmf{ _pmf }
    {
    }
    R Call() { return (t.*pmf)(p1, p2, p3, p4, p5); }
};


template <class T, class R, class P1, class P2, class P3, class P4, class P5, class P6>
class GFC_6P: public GFC<R>
{
    using PMF = R (STDMETHODCALLTYPE T::*)(P1, P2, P3, P4, P5, P6);
    T& t;
    P1 p1;
    P2 p2;
    P3 p3;
    P4 p4;
    P5 p5;
    P6 p6;
    PMF pmf;

public:
    GFC_6P(const char* s, T& _t, P1 _p1, P2 _p2, P3 _p3, P4 _p4, P5 _p5, P6 _p6, R e, PMF _pmf):
        GFC<R>{ s, e }, t{ _t }, p1{ _p1 }, p2{ _p2 }, p3{ _p3 }, p4{ _p4 },
        p5{ _p5 }, p6{ _p6 }, pmf{ _pmf }
    {
    }
    R Call() { return (t.*pmf)(p1, p2, p3, p4, p5, p6); }
};


}


namespace GuardedCallHelpers
{

template <
    class Obj,
    typename MF,
    typename R>
class MemFunCommon: private WinUtil::GuardedFunctionCall
{
public:
    R Call()
    {
        Execute();
        return itsR;
    }

protected:
    MemFunCommon(const char* caller, Obj& obj, MF mf, R r):
        GuardedFunctionCall{ caller },
        itsObj{ obj },
        itsMF{ mf },
        itsR{ r }
    {
    }

    Obj& itsObj;
    MF itsMF;
    R itsR;
};


template <
    class Obj,
    typename MF>
class VoidMemFunCommon: private WinUtil::GuardedFunctionCall
{
public:
    void Call()
    {
        Execute();
    }

protected:
    VoidMemFunCommon(const char* caller, Obj& obj, MF mf):
        GuardedFunctionCall{ caller },
        itsObj{ obj },
        itsMF{ mf }
    {
    }

    Obj& itsObj;
    MF itsMF;
};


template <
    class Obj, typename MF, typename R>
class MemFun0: public MemFunCommon<Obj, MF, R>
{
public:
    MemFun0(const char* caller, Obj& obj, MF mf, R r):
        MemFunCommon<Obj, MF, R>{ caller, obj, mf, r }
    {
    }

protected:
    void ImplementCall() { this->itsR = (this->itsObj.*this->itsMF)(); }
};


template <
    class Obj, typename MF>
class VoidMemFun0: public VoidMemFunCommon<Obj, MF>
{
public:
    VoidMemFun0(const char* caller, Obj& obj, MF mf):
        VoidMemFunCommon<Obj, MF>{ caller, obj, mf }
    {
    }

protected:
    void ImplementCall() { (this->itsObj.*this->itsMF)(); }
};


template <
    class Obj, typename MF, typename R,
    typename P1>
class MemFun1: public MemFunCommon<Obj, MF, R>
{
public:
    MemFun1(const char* caller, Obj& obj, MF mf, P1 p1, R r):
        MemFunCommon<Obj, MF, R>{ caller, obj, mf, r },
        itsP1{ p1 }
    {
    }

protected:
    void ImplementCall() { this->itsR = (this->itsObj.*this->itsMF)(itsP1); }
    P1 itsP1;
};


template <
    class Obj, typename MF, typename R,
    typename P1, typename P2>
class MemFun2: public MemFunCommon<Obj, MF, R>
{
public:
    MemFun2(const char* caller, Obj& obj, MF mf, P1 p1, P2 p2, R r):
        MemFunCommon<Obj, MF, R>{ caller, obj, mf, r },
        itsP1{ p1 }, itsP2{ p2 }
    {
    }

private:
    void ImplementCall() { this->itsR = (this->itsObj.*this->itsMF)(itsP1, itsP2); }
    P1 itsP1;
    P2 itsP2;
};


template <
    class Obj, typename MF, typename R,
    typename P1, typename P2, typename P3>
class MemFun3: public MemFunCommon<Obj, MF, R>
{
public:
    MemFun3(const char* caller, Obj& obj, MF mf, P1 p1, P2 p2, P3 p3, R r):
        MemFunCommon<Obj, MF, R>{ caller, obj, mf, r },
        itsP1{ p1 }, itsP2{ p2 }, itsP3{ p3 }
    {
    }

private:
    void ImplementCall() { this->itsR = (this->itsObj.*this->itsMF)(itsP1, itsP2, itsP3); }
    P1 itsP1;
    P2 itsP2;
    P3 itsP3;
};


template <
    class Obj, typename MF, typename R,
    typename P1, typename P2, typename P3, typename P4>
class MemFun4: public MemFunCommon<Obj, MF, R>
{
public:
    MemFun4(const char* caller, Obj& obj, MF mf, P1 p1, P2 p2, P3 p3, P4 p4, R r):
        MemFunCommon<Obj, MF, R>{ caller, obj, mf, r },
        itsP1{ p1 }, itsP2{ p2 }, itsP3{ p3 }, itsP4{ p4 }
    {
    }

private:
    void ImplementCall() { this->itsR = (this->itsObj.*this->itsMF)(itsP1, itsP2, itsP3, itsP4); }
    P1 itsP1;
    P2 itsP2;
    P3 itsP3;
    P4 itsP4;
};


template <
    typename F,
    typename R>
class FunCommon: private WinUtil::GuardedFunctionCall
{
public:
    R Call()
    {
        Execute();
        return itsR;
    }

protected:
    FunCommon(const char* caller, F f, R r):
        GuardedFunctionCall{ caller },
        itsF{ f },
        itsR{ r }
    {
    }

    F itsF;
    R itsR;
};


template <
    typename F, typename R>
class Fun0: public FunCommon<F, R>
{
public:
    Fun0(const char* caller, F f, R r):
        FunCommon<F, R>{ caller, f, r }
    {
    }

protected:
    void ImplementCall() { this->itsR = (*this->itsF)(); }
};


template <
    typename F, typename R,
    typename P1>
class Fun1: public FunCommon<F, R>
{
public:
    Fun1(const char* caller, F f, P1 p1, R r):
        FunCommon<F, R>{ caller, f, r },
        itsP1{ p1 }
    {
    }

protected:
    void ImplementCall() { this->itsR = (*this->itsF)(itsP1); }
    P1 itsP1;
};


template <
    typename F, typename R,
    typename P1, typename P2>
class Fun2: public FunCommon<F, R>
{
public:
    Fun2(const char* caller, F f, P1 p1, P2 p2, R r):
        FunCommon<F, R>{ caller, f, r },
        itsP1{ p1 }, itsP2{ p2 }
    {
    }

protected:
    void ImplementCall() { this->itsR = (*this->itsF)(itsP1, itsP2); }
    P1 itsP1;
    P2 itsP2;
};


template <
    typename F, typename R,
    typename P1, typename P2, typename P3>
class Fun3: public FunCommon<F, R>
{
public:
    Fun3(const char* caller, F f, P1 p1, P2 p2, P3 p3, R r):
        FunCommon<F, R>{ caller, f, r },
        itsP1{ p1 }, itsP2{ p2 }, itsP3{ p3 }
    {
    }

protected:
    void ImplementCall() { this->itsR = (*this->itsF)(itsP1, itsP2, itsP3); }
    P1 itsP1;
    P2 itsP2;
    P3 itsP3;
};


template <
    typename F, typename R,
    typename P1, typename P2, typename P3, typename P4>
class Fun4: public FunCommon<F, R>
{
public:
    Fun4(const char* caller, F f, P1 p1, P2 p2, P3 p3, P4 p4, R r):
        FunCommon<F, R>{ caller, f, r },
        itsP1{ p1 }, itsP2{ p2 }, itsP3{ p3 }, itsP4{ p4 }
    {
    }

protected:
    void ImplementCall() { this->itsR = (*this->itsF)(itsP1, itsP2, itsP3, itsP4); }
    P1 itsP1;
    P2 itsP2;
    P3 itsP3;
    P4 itsP4;
};

}


export namespace GuardedCallHelpers
{

template <
    class Obj>
inline void call(const char* s, Obj& obj, void (Obj::*mf)())
{
    VoidMemFun0<Obj, void (Obj::*)()>(s, obj, mf).Call();
}


template <
    class Obj>
inline void call(const char* s, const Obj& obj, void (Obj::*mf)() const)
{
    VoidMemFun0<const Obj, void (Obj::*)() const>(s, obj, mf).Call();
}


template <
    class Obj, typename R>
inline R call(const char* s, Obj& obj, R (Obj::*mf)(), R defRes)
{
    return MemFun0<Obj, R (Obj::*)(), R>(s, obj, mf, defRes).Call();
}


template <
    class Obj, typename R>
inline R call(const char* s, const Obj& obj, R (Obj::*mf)() const, R defRes)
{
    return MemFun0<const Obj, R (Obj::*)() const, R>(s, obj, mf, defRes).Call();
}


template <
    class Obj, typename R,
    typename P1, typename P1_>
inline R call(const char* s, Obj& obj, R (Obj::*mf)(P1), P1_ p1, R defRes)
{
    return MemFun1<Obj, R (Obj::*)(P1), R, P1_>(s, obj, mf, p1, defRes).Call();
}


template <
    class Obj, typename R,
    typename P1, typename P1_>
inline R call(const char* s, const Obj& obj, R (Obj::*mf)(P1) const, P1_ p1, R defRes)
{
    return MemFun1<const Obj, R (Obj::*)(P1) const, R, P1_>(s, obj, mf, p1, defRes).Call();
}


template <
    class Obj, typename R,
    typename P1, typename P1_,
    typename P2, typename P2_>
inline R call(const char* s, Obj& obj, R (Obj::*mf)(P1, P2),
    P1_ p1, P2_ p2, R defRes)
{
    return MemFun2<Obj, R (Obj::*)(P1, P2), R, P1_, P2_>(
        s, obj, mf, p1, p2, defRes)
        .Call();
}

template <
    class Obj, typename R,
    typename P1, typename P1_,
    typename P2, typename P2_>
inline R call(const char* s, const Obj& obj, R (Obj::*mf)(P1, P2) const,
    P1_ p1, P1_ p2, R defRes)
{
    return MemFun2<const Obj, R (Obj::*)(P1, P2) const, R, P1_, P2_>(
        s, obj, mf, p1, p2, defRes)
        .Call();
}


template <
    class Obj, typename R,
    typename P1, typename P1_,
    typename P2, typename P2_,
    typename P3, typename P3_>
inline R call(const char* s, Obj& obj, R (Obj::*mf)(P1, P2, P3),
    P1_ p1, P2_ p2, P3_ p3, R defRes)
{
    return MemFun3<Obj, R (Obj::*)(P1, P2, P3), R, P1_, P2_, P3_>(
        s, obj, mf, p1, p2, p3, defRes)
        .Call();
}

template <
    class Obj, typename R,
    typename P1, typename P1_,
    typename P2, typename P2_,
    typename P3, typename P3_>
inline R call(const char* s, const Obj& obj, R (Obj::*mf)(P1, P2, P3) const,
    P1_ p1, P2_ p2, P3_ p3, R defRes)
{
    return MemFun3<const Obj, R (Obj::*)(P1, P2, P3) const, R, P1_, P2_, P3_>(
        s, obj, mf, p1, p2, p3, defRes)
        .Call();
}


template <
    class Obj, typename R,
    typename P1, typename P1_,
    typename P2, typename P2_,
    typename P3, typename P3_,
    typename P4, typename P4_>
inline R call(const char* s, Obj& obj, R (Obj::*mf)(P1, P2, P3, P4),
    P1_ p1, P2_ p2, P3_ p3, P4_ p4, R defRes)
{
    return MemFun4<Obj, R (Obj::*)(P1, P2, P3, P4), R, P1_, P2_, P3_, P4_>(
        s, obj, mf, p1, p2, p3, p4, defRes)
        .Call();
}

template <
    class Obj, typename R,
    typename P1, typename P1_,
    typename P2, typename P2_,
    typename P3, typename P3_,
    typename P4, typename P4_>
inline R call(const char* s, const Obj& obj, R (Obj::*mf)(P1, P2, P3, P4) const,
    P1_ p1, P2_ p2, P3_ p3, P4_ p4, R defRes)
{
    return MemFun4<const Obj, R (Obj::*)(P1, P2, P3, P4) const, R, P1_, P2_, P3_, P4_>(
        s, obj, mf, p1, p2, p3, p4, defRes)
        .Call();
}


template <
    typename R>
inline R call(const char* s, R (*f)(), R defRes)
{
    return Fun0<R (*)(), R>(s, f, defRes).Call();
}


template <
    typename R,
    typename P1, typename P1_>
inline R call(const char* s, R (*f)(P1), P1_ p1, R defRes)
{
    return Fun1<R (*)(P1), R, P1_>(s, f, p1, defRes).Call();
}


template <
    typename R,
    typename P1, typename P1_,
    typename P2, typename P2_>
inline R call(const char* s, R (*f)(P1, P2), P1_ p1, P2_ p2, R defRes)
{
    return Fun2<R (*)(P1, P2), R, P1_, P2_>(s, f, p1, p2, defRes).Call();
}


template <
    typename R,
    typename P1, typename P1_,
    typename P2, typename P2_,
    typename P3, typename P3_>
inline R call(const char* s, R (*f)(P1, P2, P3), P1_ p1, P2_ p2, P3_ p3, R defRes)
{
    return Fun3<R (*)(P1, P2, P3), R, P1_, P2_, P3_>(s, f, p1, p2, p3, defRes).Call();
}


template <
    typename R,
    typename P1, typename P1_,
    typename P2, typename P2_,
    typename P3, typename P3_,
    typename P4, typename P4_>
inline R call(const char* s, R (*f)(P1, P2, P3, P4), P1_ p1, P2_ p2, P3_ p3, P4_ p4, R defRes)
{
    return Fun4<R (*)(P1, P2, P3, P4), R, P1_, P2_, P3_, P4_>(s, f, p1, p2, p3, p4, defRes).Call();
}


}

}
