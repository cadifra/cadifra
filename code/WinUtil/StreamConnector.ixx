/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.StreamConnector;

import d1.RefCounter;

import std;


namespace WinUtil
{

export class StreamConnector: public ::IStream
{
    d1::RefCounter<StreamConnector> refCounter_;
    std::wostream& OStream_;
    ULONGLONG position_ = {};
    ULONGLONG size_= {};

public:
    StreamConnector(std::wostream& os);

    ///////////////////////////////////////////////////////
    // IUnknown methods

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void** pvObject) override;

    virtual ULONG STDMETHODCALLTYPE AddRef() override;

    virtual ULONG STDMETHODCALLTYPE Release() override;


    ///////////////////////////////////////////////////////
    // ISequentialStream methods

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Read(
        /* [length_is][size_is][out] */ void* pv,
        /* [in] */ ULONG cb,
        /* [out] */ ULONG* pcbRead) override;

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Write(
        /* [size_is][in] */ const void* pv,
        /* [in] */ ULONG cb,
        /* [out] */ ULONG* pcbWritten) override;


    ///////////////////////////////////////////////////////
    // IStream methods

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Seek(
        /* [in] */ LARGE_INTEGER dlibMove,
        /* [in] */ DWORD dwOrigin,
        /* [out] */ ULARGE_INTEGER* plibNewPosition) override;

    virtual HRESULT STDMETHODCALLTYPE SetSize(
        /* [in] */ ULARGE_INTEGER libNewSize) override;

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE CopyTo(
        /* [unique][in] */ IStream* pstm,
        /* [in] */ ULARGE_INTEGER cb,
        /* [out] */ ULARGE_INTEGER* pcbRead,
        /* [out] */ ULARGE_INTEGER* pcbWritten) override;

    virtual HRESULT STDMETHODCALLTYPE Commit(
        /* [in] */ DWORD grfCommitFlags) override;

    virtual HRESULT STDMETHODCALLTYPE Revert() override;

    virtual HRESULT STDMETHODCALLTYPE LockRegion(
        /* [in] */ ULARGE_INTEGER libOffset,
        /* [in] */ ULARGE_INTEGER cb,
        /* [in] */ DWORD dwLockType) override;

    virtual HRESULT STDMETHODCALLTYPE UnlockRegion(
        /* [in] */ ULARGE_INTEGER libOffset,
        /* [in] */ ULARGE_INTEGER cb,
        /* [in] */ DWORD dwLockType) override;

    virtual HRESULT STDMETHODCALLTYPE Stat(
        /* [out] */ STATSTG* pstatstg,
        /* [in] */ DWORD grfStatFlag) override;

    virtual HRESULT STDMETHODCALLTYPE Clone(
        /* [out] */ IStream** ppstm) override;
};

}

module : private;


namespace WinUtil
{

using C = StreamConnector;


C::StreamConnector(std::wostream& os):
    OStream_{ os }
{
}


// IUnknown methods

ULONG C::AddRef()
{
    return refCounter_.AddRef();
}


ULONG C::Release()
{
    return refCounter_.Release(this);
}


HRESULT C::QueryInterface(
    REFIID riid,
    void** pvObject)
{
    if (riid == IID_IUnknown)
        *pvObject = static_cast<IUnknown*>(this);
    else if (riid == IID_IStream)
        *pvObject = static_cast<IStream*>(this);
    else if (riid == IID_ISequentialStream)
        *pvObject = static_cast<ISequentialStream*>(this);
    else
    {
        *pvObject = 0;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}



// ISequentialStream methods

HRESULT C::Read(void*, ULONG, ULONG*)
{
    return E_NOTIMPL;
}


HRESULT C::Write(const void* pv, ULONG cb, ULONG* pcbWritten)
{
    ULONG cb2 = cb / sizeof(wchar_t);

    if (cb2)
    {
        // do not copy null bytes:

        const wchar_t* i = static_cast<const wchar_t*>(pv);

        ULONG len = 0;
        for (; len <= cb2; ++len, ++i)
        {
            if (*i == 0)
                break;
        }
        cb2 = len;

        OStream_.write(static_cast<const wchar_t*>(pv), cb2);
    }

    if (pcbWritten)
        *pcbWritten = cb;

    position_ += cb2 * sizeof(wchar_t);

    if (position_ > size_)
        size_ = position_;

    return S_OK;
}


// IStream methods

HRESULT C::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin,
	ULARGE_INTEGER* plibNewPosition)
{
    switch (dwOrigin)
    {
    case STREAM_SEEK_SET:
        position_ = dlibMove.QuadPart;
        break;

    case STREAM_SEEK_CUR:
        position_ += dlibMove.QuadPart;
        break;

    case STREAM_SEEK_END:
        position_ = size_ + dlibMove.QuadPart;
        break;

    default:
        return STG_E_INVALIDFUNCTION;
    }

    if (position_ > size_)
        size_ = position_;

    if (plibNewPosition)
        plibNewPosition->QuadPart = position_;

    return S_OK;
}


HRESULT C::SetSize(ULARGE_INTEGER libNewSize)
{
    size_ = libNewSize.QuadPart;
    return S_OK;
}


HRESULT C::CopyTo(IStream*, ULARGE_INTEGER cb, ULARGE_INTEGER*, ULARGE_INTEGER*)
{
    return E_NOTIMPL;
}


HRESULT C::Commit(DWORD)
{
    return E_NOTIMPL;
}


HRESULT C::Revert()
{
    return E_NOTIMPL;
}


HRESULT C::LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
{
    return E_NOTIMPL;
}


HRESULT C::UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
{
    return E_NOTIMPL;
}


HRESULT C::Stat(STATSTG*, DWORD)
{
    return E_NOTIMPL;
}


HRESULT C::Clone(IStream**)
{
    return E_NOTIMPL;
}

}
