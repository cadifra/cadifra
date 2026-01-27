/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil.StreamConnector;


namespace WinUtil
{

namespace
{
using C = StreamConnector;
}


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
