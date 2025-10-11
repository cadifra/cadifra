/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil:StreamConnector;

import d1.RefCounter;

import std;


namespace WinUtil
{

export class StreamConnector: public ::IStream
{
    d1::RefCounter<StreamConnector> itsRefCounter;
    std::wostream& itsOStream;
    ULONGLONG itsPosition = {};
    ULONGLONG itsSize= {};

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
