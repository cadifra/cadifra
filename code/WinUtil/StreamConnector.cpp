/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil;


namespace WinUtil
{

namespace
{
using C = StreamConnector;
}


C::StreamConnector(std::wostream& os):
    itsOStream{ os }
{
}


// IUnknown methods

ULONG C::AddRef()
{
    return itsRefCounter.AddRef();
}


ULONG C::Release()
{
    return itsRefCounter.Release(this);
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

        itsOStream.write(static_cast<const wchar_t*>(pv), cb2);
    }

    if (pcbWritten)
        *pcbWritten = cb;

    itsPosition += cb2 * sizeof(wchar_t);

    if (itsPosition > itsSize)
        itsSize = itsPosition;

    return S_OK;
}


// IStream methods

HRESULT C::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin,
	ULARGE_INTEGER* plibNewPosition)
{
    switch (dwOrigin)
    {
    case STREAM_SEEK_SET:
        itsPosition = dlibMove.QuadPart;
        break;

    case STREAM_SEEK_CUR:
        itsPosition += dlibMove.QuadPart;
        break;

    case STREAM_SEEK_END:
        itsPosition = itsSize + dlibMove.QuadPart;
        break;

    default:
        return STG_E_INVALIDFUNCTION;
    }

    if (itsPosition > itsSize)
        itsSize = itsPosition;

    if (plibNewPosition)
        plibNewPosition->QuadPart = itsPosition;

    return S_OK;
}


HRESULT C::SetSize(ULARGE_INTEGER libNewSize)
{
    itsSize = libNewSize.QuadPart;
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
