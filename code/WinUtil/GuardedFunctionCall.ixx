/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.GuardedFunctionCall;

import WinUtil.ComException;

import std;


namespace WinUtil
{

export class GuardedFunctionCall
{
    const char* caller_;

public:
    enum class Result
    {
        ok = 0,
        exception_catched
    };

    GuardedFunctionCall(const char* caller) throw():
        caller_{ caller }
    {
    }
    virtual ~GuardedFunctionCall() throw() = default;

    virtual Result execute() throw(); // calls this->implementCall()

private:
    virtual void implementCall() = 0; /* may throw any exception */
    Result callL2();
    Result callL3();

    void showErrorBox(const std::exception& e) const;
};


export template <class T>
class GuardedFunctionCallRet: public GuardedFunctionCall
{
    T res_;
    T failureRes_;

public:
    GuardedFunctionCallRet(const char* caller, T failure_res) throw():
        GuardedFunctionCall{ caller }, res_{ failure_res }, failureRes_{ failure_res }
    {
    }
    Result execute() throw() override
    {
        Result r = GuardedFunctionCall::execute();
        if (r != Result::ok)
            res_ = failureRes_; // exception occurred
        return r;
    }
    T res() const throw() { return res_; }

private:
    void implementCall() override
    {
        res_ = implementCallRet(); /* may throw any exception */
    }
    virtual T implementCallRet() = 0;
};


export namespace GuardedComFunctionCall
{

// helper class:

template <class R>
class GFC: public GuardedFunctionCallRet<R>
{
    virtual R call() = 0;
    R implementCallRet() { return call(); }

public:
    GFC(const char* s, R e):
        GuardedFunctionCallRet<R>{ s, e }
    {
    }
    R operator()()
    {
        this->execute();
        return this->res();
    }
};


template <>
class GFC<HRESULT>: public GuardedFunctionCallRet<HRESULT>
{
    virtual HRESULT call() = 0;
    HRESULT implementCallRet()
    {
        try
        {
            return call();
        }
        catch (ComException& e)
        {
            return e.getHRESULT();
        }
    }

public:
    GFC(const char* s, HRESULT e):
        GuardedFunctionCallRet<HRESULT>{ s, e }
    {
    }
    HRESULT operator()()
    {
        this->execute();
        return this->res();
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
    T& t_;
    PMF pmf_;

public:
    GFC_0P(const char* s, T& t, R e, PMF pmf):
        GFC<R>{ s, e }, t_{ t }, pmf_{ pmf }
    {
    }
    R call() { return (t_.*pmf_)(); }
};


template <class T, class R, class P1>
class GFC_1P: public GFC<R>
{
    using PMF = R (STDMETHODCALLTYPE T::*)(P1);
    T& t_;
    P1 p1_;
    PMF pmf_;

public:
    GFC_1P(const char* s, T& t, P1 p1, R e, PMF pmf):
        GFC<R>{ s, e }, t_{ t }, p1_{ p1 }, pmf_{ pmf }
    {
    }
    R call() { return (t_.*pmf_)(p1_); }
};


template <class T, class R, class P1, class P2>
class GFC_2P: public GFC<R>
{
    using PMF = R (STDMETHODCALLTYPE T::*)(P1, P2);
    T& t_;
    P1 p1_;
    P2 p2_;
    PMF pmf_;

public:
    GFC_2P(const char* s, T& t, P1 p1, P2 p2, R e, PMF pmf):
        GFC<R>{ s, e }, t_{ t }, p1_{ p1 }, p2_{ p2 }, pmf_{ pmf }
    {
    }
    R call() { return (t_.*pmf_)(p1_, p2_); }
};


template <class T, class R, class P1, class P2, class P3>
class GFC_3P: public GFC<R>
{
    using PMF = R (STDMETHODCALLTYPE T::*)(P1, P2, P3);
    T& t_;
    P1 p1_;
    P2 p2_;
    P3 p3_;
    PMF pmf_;

public:
    GFC_3P(const char* s, T& t, P1 p1, P2 p2, P3 p3, R e, PMF pmf):
        GFC<R>{ s, e }, t_{ t }, p1_{ p1 }, p2_{ p2 }, p3_{ p3 }, pmf_{ pmf }
    {
    }
    R call() { return (t_.*pmf_)(p1_, p2_, p3_); }
};


template <class T, class R, class P1, class P2, class P3, class P4>
class GFC_4P: public GFC<R>
{
    using PMF = R (STDMETHODCALLTYPE T::*)(P1, P2, P3, P4);
    T& t_;
    P1 p1_;
    P2 p2_;
    P3 p3_;
    P4 p4_;
    PMF pmf_;

public:
    GFC_4P(const char* s, T& t, P1 p1, P2 p2, P3 p3, P4 p4, R e, PMF pmf):
        GFC<R>{ s, e }, t_{ t }, p1_{ p1 }, p2_{ p2 }, p3_{ p3 }, p4_{ p4 }, pmf_{ pmf }
    {
    }
    R call() { return (t_.*pmf_)(p1_, p2_, p3_, p4_); }
};


template <class T, class R, class P1, class P2, class P3, class P4, class P5>
class GFC_5P: public GFC<R>
{
    using PMF = R (STDMETHODCALLTYPE T::*)(P1, P2, P3, P4, P5);
    T& t_;
    P1 p1_;
    P2 p2_;
    P3 p3_;
    P4 p4_;
    P5 p5_;
    PMF pmf_;

public:
    GFC_5P(const char* s, T& t, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, R e, PMF pmf):
        GFC<R>{ s, e }, t_{ t }, p1_{ p1 }, p2_{ p2 }, p3_{ p3 },
        p4_{ p4 }, p5_{ p5 }, pmf_{ pmf }
    {
    }
    R call() { return (t_.*pmf_)(p1_, p2_, p3_, p4_, p5_); }
};


template <class T, class R, class P1, class P2, class P3, class P4, class P5, class P6>
class GFC_6P: public GFC<R>
{
    using PMF = R (STDMETHODCALLTYPE T::*)(P1, P2, P3, P4, P5, P6);
    T& t_;
    P1 p1_;
    P2 p2_;
    P3 p3_;
    P4 p4_;
    P5 p5_;
    P6 p6_;
    PMF pmf_;

public:
    GFC_6P(const char* s, T& t, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, R e, PMF pmf):
        GFC<R>{ s, e }, t_{ t }, p1_{ p1 }, p2_{ p2 }, p3_{ p3 }, p4_{ p4 },
        p5_{ p5 }, p6_{ p6 }, pmf_{ pmf }
    {
    }
    R call() { return (t_.*pmf_)(p1_, p2_, p3_, p4_, p5_, p6_); }
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
    R call()
    {
        execute();
        return R_;
    }

protected:
    MemFunCommon(const char* caller, Obj& obj, MF mf, R r):
        GuardedFunctionCall{ caller },
        obj_{ obj },
        MF_{ mf },
        R_{ r }
    {
    }

    Obj& obj_;
    MF MF_;
    R R_;
};


template <
    class Obj,
    typename MF>
class VoidMemFunCommon: private WinUtil::GuardedFunctionCall
{
public:
    void call()
    {
        execute();
    }

protected:
    VoidMemFunCommon(const char* caller, Obj& obj, MF mf):
        GuardedFunctionCall{ caller },
        obj_{ obj },
        MF_{ mf }
    {
    }

    Obj& obj_;
    MF MF_;
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
    void implementCall() { this->R_ = (this->obj_.*this->MF_)(); }
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
    void implementCall() { (this->obj_.*this->MF_)(); }
};


template <
    class Obj, typename MF, typename R,
    typename P1>
class MemFun1: public MemFunCommon<Obj, MF, R>
{
public:
    MemFun1(const char* caller, Obj& obj, MF mf, P1 p1, R r):
        MemFunCommon<Obj, MF, R>{ caller, obj, mf, r },
        P1_{ p1 }
    {
    }

protected:
    void implementCall() { this->R_ = (this->obj_.*this->MF_)(P1_); }
    P1 P1_;
};


template <
    class Obj, typename MF, typename R,
    typename P1, typename P2>
class MemFun2: public MemFunCommon<Obj, MF, R>
{
public:
    MemFun2(const char* caller, Obj& obj, MF mf, P1 p1, P2 p2, R r):
        MemFunCommon<Obj, MF, R>{ caller, obj, mf, r },
        P1_{ p1 }, P2_{ p2 }
    {
    }

private:
    void implementCall() { this->R_ = (this->obj_.*this->MF_)(P1_, P2_); }
    P1 P1_;
    P2 P2_;
};


template <
    class Obj, typename MF, typename R,
    typename P1, typename P2, typename P3>
class MemFun3: public MemFunCommon<Obj, MF, R>
{
public:
    MemFun3(const char* caller, Obj& obj, MF mf, P1 p1, P2 p2, P3 p3, R r):
        MemFunCommon<Obj, MF, R>{ caller, obj, mf, r },
        P1_{ p1 }, P2_{ p2 }, P3_{ p3 }
    {
    }

private:
    void implementCall() { this->R_ = (this->obj_.*this->MF_)(P1_, P2_, P3_); }
    P1 P1_;
    P2 P2_;
    P3 P3_;
};


template <
    class Obj, typename MF, typename R,
    typename P1, typename P2, typename P3, typename P4>
class MemFun4: public MemFunCommon<Obj, MF, R>
{
public:
    MemFun4(const char* caller, Obj& obj, MF mf, P1 p1, P2 p2, P3 p3, P4 p4, R r):
        MemFunCommon<Obj, MF, R>{ caller, obj, mf, r },
        P1_{ p1 }, P2_{ p2 }, P3_{ p3 }, P4_{ p4 }
    {
    }

private:
    void implementCall() { this->R_ = (this->obj_.*this->MF_)(P1_, P2_, P3_, P4_); }
    P1 P1_;
    P2 P2_;
    P3 P3_;
    P4 P4_;
};


template <
    typename F,
    typename R>
class FunCommon: private WinUtil::GuardedFunctionCall
{
public:
    R call()
    {
        execute();
        return R_;
    }

protected:
    FunCommon(const char* caller, F f, R r):
        GuardedFunctionCall{ caller },
        F_{ f },
        R_{ r }
    {
    }

    F F_;
    R R_;
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
    void implementCall() { this->R_ = (*this->F_)(); }
};


template <
    typename F, typename R,
    typename P1>
class Fun1: public FunCommon<F, R>
{
public:
    Fun1(const char* caller, F f, P1 p1, R r):
        FunCommon<F, R>{ caller, f, r },
        P1_{ p1 }
    {
    }

protected:
    void implementCall() { this->R_ = (*this->F_)(P1_); }
    P1 P1_;
};


template <
    typename F, typename R,
    typename P1, typename P2>
class Fun2: public FunCommon<F, R>
{
public:
    Fun2(const char* caller, F f, P1 p1, P2 p2, R r):
        FunCommon<F, R>{ caller, f, r },
        P1_{ p1 }, P2_{ p2 }
    {
    }

protected:
    void implementCall() { this->R_ = (*this->F_)(P1_, P2_); }
    P1 P1_;
    P2 P2_;
};


template <
    typename F, typename R,
    typename P1, typename P2, typename P3>
class Fun3: public FunCommon<F, R>
{
public:
    Fun3(const char* caller, F f, P1 p1, P2 p2, P3 p3, R r):
        FunCommon<F, R>{ caller, f, r },
        P1_{ p1 }, P2_{ p2 }, P3_{ p3 }
    {
    }

protected:
    void implementCall() { this->R_ = (*this->F_)(P1_, P2_, P3_); }
    P1 P1_;
    P2 P2_;
    P3 P3_;
};


template <
    typename F, typename R,
    typename P1, typename P2, typename P3, typename P4>
class Fun4: public FunCommon<F, R>
{
public:
    Fun4(const char* caller, F f, P1 p1, P2 p2, P3 p3, P4 p4, R r):
        FunCommon<F, R>{ caller, f, r },
        P1_{ p1 }, P2_{ p2 }, P3_{ p3 }, P4_{ p4 }
    {
    }

protected:
    void implementCall() { this->R_ = (*this->F_)(P1_, P2_, P3_, P4_); }
    P1 P1_;
    P2 P2_;
    P3 P3_;
    P4 P4_;
};

}


export namespace GuardedCallHelpers
{

template <
    class Obj>
inline void call(const char* s, Obj& obj, void (Obj::*mf)())
{
    VoidMemFun0<Obj, void (Obj::*)()>(s, obj, mf).call();
}


template <
    class Obj>
inline void call(const char* s, const Obj& obj, void (Obj::*mf)() const)
{
    VoidMemFun0<const Obj, void (Obj::*)() const>(s, obj, mf).call();
}


template <
    class Obj, typename R>
inline R call(const char* s, Obj& obj, R (Obj::*mf)(), R defRes)
{
    return MemFun0<Obj, R (Obj::*)(), R>(s, obj, mf, defRes).call();
}


template <
    class Obj, typename R>
inline R call(const char* s, const Obj& obj, R (Obj::*mf)() const, R defRes)
{
    return MemFun0<const Obj, R (Obj::*)() const, R>(s, obj, mf, defRes).call();
}


template <
    class Obj, typename R,
    typename P1, typename P1_>
inline R call(const char* s, Obj& obj, R (Obj::*mf)(P1), P1_ p1, R defRes)
{
    return MemFun1<Obj, R (Obj::*)(P1), R, P1_>(s, obj, mf, p1, defRes).call();
}


template <
    class Obj, typename R,
    typename P1, typename P1_>
inline R call(const char* s, const Obj& obj, R (Obj::*mf)(P1) const, P1_ p1, R defRes)
{
    return MemFun1<const Obj, R (Obj::*)(P1) const, R, P1_>(s, obj, mf, p1, defRes).call();
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
        .call();
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
        .call();
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
        .call();
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
        .call();
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
        .call();
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
        .call();
}


template <
    typename R>
inline R call(const char* s, R (*f)(), R defRes)
{
    return Fun0<R (*)(), R>(s, f, defRes).call();
}


template <
    typename R,
    typename P1, typename P1_>
inline R call(const char* s, R (*f)(P1), P1_ p1, R defRes)
{
    return Fun1<R (*)(P1), R, P1_>(s, f, p1, defRes).call();
}


template <
    typename R,
    typename P1, typename P1_,
    typename P2, typename P2_>
inline R call(const char* s, R (*f)(P1, P2), P1_ p1, P2_ p2, R defRes)
{
    return Fun2<R (*)(P1, P2), R, P1_, P2_>(s, f, p1, p2, defRes).call();
}


template <
    typename R,
    typename P1, typename P1_,
    typename P2, typename P2_,
    typename P3, typename P3_>
inline R call(const char* s, R (*f)(P1, P2, P3), P1_ p1, P2_ p2, P3_ p3, R defRes)
{
    return Fun3<R (*)(P1, P2, P3), R, P1_, P2_, P3_>(s, f, p1, p2, p3, defRes).call();
}


template <
    typename R,
    typename P1, typename P1_,
    typename P2, typename P2_,
    typename P3, typename P3_,
    typename P4, typename P4_>
inline R call(const char* s, R (*f)(P1, P2, P3, P4), P1_ p1, P2_ p2, P3_ p3, P4_ p4, R defRes)
{
    return Fun4<R (*)(P1, P2, P3, P4), R, P1_, P2_, P3_, P4_>(s, f, p1, p2, p3, p4, defRes).call();
}


}

}
