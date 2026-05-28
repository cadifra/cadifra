/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>
#include <msi.h>

export module WinUtil.MsiLib;


namespace WinUtil
{

export class MsiLib
{
    HMODULE HModule_;

public:
    MsiLib();

    MsiLib(const MsiLib&) = delete;
    MsiLib& operator=(const MsiLib&) = delete;

    ~MsiLib();

    operator bool() const { return 0 != HModule_; }

    UINT msiProvideComponent(
        LPCTSTR szProduct,   // product code in case install required
        LPCTSTR szFeature,   // feature ID in case install required
        LPCTSTR szComponent, // component ID
        DWORD dwInstallMode, // the install mode
        LPTSTR lpPathBuf,    // returned path to component key file
        DWORD* pcchPathBuf   // in/out buffer character count
    ) const;

    INSTALLSTATE msiGetComponentPath(
        LPCTSTR szProduct,   // product code for client product
        LPCTSTR szComponent, // component ID
        LPTSTR lpPathBuf,    // returned path
        DWORD* pcchBuf       // buffer character count
    ) const;

    INSTALLSTATE msiQueryFeatureState(
        LPCTSTR szProduct,
        LPCTSTR szFeature) const;

    UINT msiConfigureFeature(
        LPCTSTR szProduct,
        LPCTSTR szFeature,
        INSTALLSTATE eInstallState) const;

    UINT msiGetProductInfo(
        LPCTSTR szProduct,
        LPCTSTR szProperty,
        LPTSTR lpValueBuf,
        DWORD* pcchValueBuf) const;
};


#ifdef UNICODE
#define GETPROCADDR(s) ::GetProcAddress(HModule_, #s "W")
#else
#define GETPROCADDR(s) ::GetProcAddress(HModule_, #s "A")
#endif



MsiLib::MsiLib():
    HModule_{ ::LoadLibrary(TEXT("Msi.dll")) }
{
}


MsiLib::~MsiLib()
{
    if (HModule_)
        ::FreeLibrary(HModule_);
}


UINT MsiLib::msiProvideComponent(
    LPCTSTR szProduct,   // product code in case install required
    LPCTSTR szFeature,   // feature ID in case install required
    LPCTSTR szComponent, // component ID
    DWORD dwInstallMode, // the install mode
    LPTSTR lpPathBuf,    // returned path to component key file
    DWORD* pcchPathBuf   // in/out buffer character count
) const
{
    using fun = UINT(WINAPI*)(LPCTSTR, LPCTSTR, LPCTSTR, DWORD, LPCTSTR, DWORD*);
    return reinterpret_cast<fun>(GETPROCADDR(MsiProvideComponent))(
        szProduct, szFeature, szComponent, dwInstallMode, lpPathBuf, pcchPathBuf);
}


INSTALLSTATE MsiLib::msiGetComponentPath(
    LPCTSTR szProduct,   // product code for client product
    LPCTSTR szComponent, // component ID
    LPTSTR lpPathBuf,    // returned path
    DWORD* pcchBuf       // buffer character count
) const
{
    using fun = INSTALLSTATE(WINAPI*)(LPCTSTR, LPCTSTR, LPTSTR, DWORD*);
    return reinterpret_cast<fun>(GETPROCADDR(MsiGetComponentPath))(
        szProduct, szComponent, lpPathBuf, pcchBuf);
}


INSTALLSTATE MsiLib::msiQueryFeatureState(
    LPCTSTR szProduct,
    LPCTSTR szFeature) const
{
    using fun = INSTALLSTATE(WINAPI*)(LPCTSTR, LPCTSTR);
    return reinterpret_cast<fun>(GETPROCADDR(MsiQueryFeatureState))(
        szProduct, szFeature);
}


UINT MsiLib::msiConfigureFeature(
    LPCTSTR szProduct,
    LPCTSTR szFeature,
    INSTALLSTATE eInstallState) const
{
    using fun = UINT(WINAPI*)(LPCTSTR, LPCTSTR, INSTALLSTATE);
    return reinterpret_cast<fun>(GETPROCADDR(MsiConfigureFeature))(
        szProduct, szFeature, eInstallState);
}


UINT MsiLib::msiGetProductInfo(
    LPCTSTR szProduct,
    LPCTSTR szProperty,
    LPTSTR lpValueBuf,
    DWORD* pcchValueBuf) const
{
    using fun = UINT(WINAPI*)(LPCTSTR, LPCTSTR, LPTSTR, DWORD*);
    return reinterpret_cast<fun>(GETPROCADDR(MsiGetProductInfo))(
        szProduct, szProperty, lpValueBuf, pcchValueBuf);
}

}
