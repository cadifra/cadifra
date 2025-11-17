/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil.Clipboard;

import WinUtil.Global;

import d1.AutoComPtr;

import std;


namespace WinUtil::Clipboard
{

HRESULT copy(const void* from, size_t from_size,
    const tagFORMATETC* pFormatetc, tagSTGMEDIUM* pmedium)
{
    if (!pFormatetc || !pmedium)
        return E_INVALIDARG;

    pmedium->tymed = TYMED_NULL;
    pmedium->pstm = 0;
    pmedium->pUnkForRelease = 0;

    if (pFormatetc->tymed & TYMED_ISTREAM)
    {
        ::IStream* s;
        HRESULT res = ::CreateStreamOnHGlobal(0, true, &s);
        if (res != S_OK)
            return res;
        res = s->Write(from, static_cast<ULONG>(from_size), 0);
        if (res != S_OK)
        {
            s->Release();
            return res;
        }
        pmedium->tymed = TYMED_ISTREAM;
        pmedium->pstm = s;
        return S_OK;
    }

    if (pFormatetc->tymed & TYMED_HGLOBAL)
    {
        auto g = WinUtil::GlobalOwner{ ::GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, from_size) };
        if (!g.Get())
            return E_OUTOFMEMORY;
        auto mem = WinUtil::GlobalLocker<BYTE>{ g.Get() };
        if (!mem)
            return E_UNEXPECTED;
        memcpy(mem, from, from_size);
        pmedium->tymed = TYMED_HGLOBAL;
        pmedium->hGlobal = g.Release();
        return S_OK;
    }

    return E_NOTIMPL;
}


HRESULT copy(::IStream* from, ::IStream* to)
{
    if (!from || !to)
        return E_INVALIDARG;

    ULARGE_INTEGER max;
    max.QuadPart = std::numeric_limits<ULONGLONG>::max();

    LARGE_INTEGER null;
    null.QuadPart = 0;

    // set the seek pointer to the beginning
    HRESULT res = from->Seek(null, STREAM_SEEK_SET, 0);
    if (FAILED(res))
        return res;

    return from->CopyTo(to, max, 0, 0);
}


HRESULT copy(HGLOBAL from, ::IStream* to)
{
    if (!from || !to)
        return E_INVALIDARG;
    auto s = d1::AutoComPtr<::IStream>{};
    HRESULT res = ::CreateStreamOnHGlobal(from, FALSE, &s);
    if (FAILED(res))
        return res;
    return copy(s, to);
}


HRESULT copy(const STGMEDIUM& from, ::IStream* to)
{
    if (from.tymed == TYMED_ISTREAM)
        return copy(from.pstm, to);
    else if (from.tymed == TYMED_HGLOBAL)
        return copy(from.hGlobal, to);

    return E_INVALIDARG;
}

}
