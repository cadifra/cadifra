/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

export module WinUtil.OwnerSTGMEDIUM;


namespace WinUtil
{

export struct OwnerSTGMEDIUM: public STGMEDIUM
{
    OwnerSTGMEDIUM();

    OwnerSTGMEDIUM(HBITMAP, IUnknown* unkForRelease = 0);
    OwnerSTGMEDIUM(LPOLESTR, IUnknown* unkForRelease = 0);
    OwnerSTGMEDIUM(IStream*, IUnknown* unkForRelease = 0);
    OwnerSTGMEDIUM(IStorage*, IUnknown* unkForRelease = 0);
    OwnerSTGMEDIUM(HENHMETAFILE, IUnknown* unkForRelease = 0);


    // HMETAFILEPICT and HGLOBAL are the same types !!!

    OwnerSTGMEDIUM(
        TYMED /* TYMED_HGLOBAL or TYMED_MFPICT */,
        HGLOBAL v, IUnknown* unkForRelease = 0);


    void Assign(HBITMAP, IUnknown* unkForRelease = 0);
    void Assign(LPOLESTR, IUnknown* unkForRelease = 0);
    void Assign(IStream*, IUnknown* unkForRelease = 0);
    void Assign(IStorage*, IUnknown* unkForRelease = 0);
    void Assign(HENHMETAFILE, IUnknown* unkForRelease = 0);

    // HMETAFILEPICT and HGLOBAL are the same types !!!

    void Assign(
        TYMED /* TYMED_HGLOBAL or TYMED_MFPICT */,
        HGLOBAL v, IUnknown* unkForRelease = 0);


    void Free();

    ~OwnerSTGMEDIUM();

    OwnerSTGMEDIUM(const OwnerSTGMEDIUM&) = delete;
    OwnerSTGMEDIUM& operator=(const OwnerSTGMEDIUM&) = delete;
};



// inline code


inline OwnerSTGMEDIUM::OwnerSTGMEDIUM()
{
    tymed = TYMED_NULL;
    pstm = 0;
    pUnkForRelease = 0;
}


inline OwnerSTGMEDIUM::OwnerSTGMEDIUM(HBITMAP v, IUnknown* u)
{
    tymed = TYMED_GDI;
    hBitmap = v;
    pUnkForRelease = u;

    if (u)
        u->AddRef();
}


inline OwnerSTGMEDIUM::OwnerSTGMEDIUM(LPOLESTR v, IUnknown* u)
{
    tymed = TYMED_FILE;
    lpszFileName = v;
    pUnkForRelease = u;

    if (u)
        u->AddRef();
}


inline OwnerSTGMEDIUM::OwnerSTGMEDIUM(IStream* v, IUnknown* u)
{
    tymed = TYMED_ISTREAM;
    pstm = v;
    pUnkForRelease = u;

    if (v)
        v->AddRef();
    if (u)
        u->AddRef();
}


inline OwnerSTGMEDIUM::OwnerSTGMEDIUM(IStorage* v, IUnknown* u)
{
    tymed = TYMED_ISTORAGE;
    pstg = v;
    pUnkForRelease = u;

    if (v)
        v->AddRef();
    if (u)
        u->AddRef();
}


inline OwnerSTGMEDIUM::OwnerSTGMEDIUM(HENHMETAFILE v, IUnknown* u)
{
    tymed = TYMED_ENHMF;
    hEnhMetaFile = v;
    pUnkForRelease = u;

    if (u)
        u->AddRef();
}


inline OwnerSTGMEDIUM::OwnerSTGMEDIUM(TYMED t, HGLOBAL v, IUnknown* u)
{
    D1_ASSERT((t == TYMED_HGLOBAL) or (t == TYMED_MFPICT));

    tymed = TYMED_HGLOBAL;
    hGlobal = v;
    pUnkForRelease = u;

    if (u)
        u->AddRef();
}


inline void OwnerSTGMEDIUM::Assign(HBITMAP v, IUnknown* u)
{
    Free();
    tymed = TYMED_GDI;
    hBitmap = v;
    pUnkForRelease = u;

    if (u)
        u->AddRef();
}


inline void OwnerSTGMEDIUM::Assign(LPOLESTR v, IUnknown* u)
{
    Free();
    tymed = TYMED_FILE;
    lpszFileName = v;
    pUnkForRelease = u;

    if (u)
        u->AddRef();
}


inline void OwnerSTGMEDIUM::Assign(IStream* v, IUnknown* u)
{
    Free();
    tymed = TYMED_ISTREAM;
    pstm = v;
    pUnkForRelease = u;

    if (v)
        v->AddRef();
    if (u)
        u->AddRef();
}


inline void OwnerSTGMEDIUM::Assign(IStorage* v, IUnknown* u)
{
    Free();
    tymed = TYMED_ISTORAGE;
    pstg = v;
    pUnkForRelease = u;

    if (v)
        v->AddRef();
    if (u)
        u->AddRef();
}


inline void OwnerSTGMEDIUM::Assign(HENHMETAFILE v, IUnknown* u)
{
    Free();
    tymed = TYMED_ENHMF;
    hEnhMetaFile = v;
    pUnkForRelease = u;

    if (u)
        u->AddRef();
}


inline void OwnerSTGMEDIUM::Assign(TYMED t, HGLOBAL v, IUnknown* u)
{
    D1_ASSERT((t == TYMED_HGLOBAL) or (t == TYMED_MFPICT));

    Free();
    tymed = TYMED_HGLOBAL;
    hGlobal = v;
    pUnkForRelease = u;

    if (u)
        u->AddRef();
}


inline void OwnerSTGMEDIUM::Free()
{
    if (tymed != TYMED_NULL)
    {
        ::ReleaseStgMedium(this);
        tymed = TYMED_NULL;
        pstm = 0;
        pUnkForRelease = 0;
    }
}

inline OwnerSTGMEDIUM::~OwnerSTGMEDIUM()
{
    Free();
}


}
