/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>
#include <objidl.h>

export module App.IFormatProvider;

namespace App
{

export class IFormatProvider
{
public:
    void AddRef();
    void Release();

    virtual int GetFormatCount() const { return 1; }

    virtual void GetFormat(int formatIndex, tagFORMATETC&) const = 0;

    virtual HRESULT GetData(
        int formatIndex,
        tagFORMATETC* pFormatetc,
        tagSTGMEDIUM* pmedium) = 0;

    virtual HRESULT GetDataHere(
        int formatIndex,
        tagFORMATETC* pFormatetc,
        tagSTGMEDIUM* pmedium) = 0;

    virtual void ClearCache() {}

    virtual ~IFormatProvider() = default;

private:
    int refCount_ = 0;
};


void IFormatProvider::AddRef()
{
    ++refCount_;
}


void IFormatProvider::Release()
{
    if (--refCount_ <= 0)
        delete this;
}

}
