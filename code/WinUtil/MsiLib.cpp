/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>
#include <msi.h>

module WinUtil.MsiLib;


namespace WinUtil
{

#ifdef UNICODE
#define GETPROCADDR(s) ::GetProcAddress(HModule_, #s "W")
#else
#define GETPROCADDR(s) ::GetProcAddress(HModule_, #s "A")
#endif

namespace
{
using C = MsiLib;
}


C::MsiLib():
    HModule_{ ::LoadLibrary(TEXT("Msi.dll")) }
{
}


C::~MsiLib()
{
    if (HModule_)
        ::FreeLibrary(HModule_);
}


UINT C::msiProvideComponent(
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


INSTALLSTATE C::msiGetComponentPath(
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


INSTALLSTATE C::msiQueryFeatureState(
    LPCTSTR szProduct,
    LPCTSTR szFeature) const
{
    using fun = INSTALLSTATE(WINAPI*)(LPCTSTR, LPCTSTR);
    return reinterpret_cast<fun>(GETPROCADDR(MsiQueryFeatureState))(
        szProduct, szFeature);
}


UINT C::msiConfigureFeature(
    LPCTSTR szProduct,
    LPCTSTR szFeature,
    INSTALLSTATE eInstallState) const
{
    using fun = UINT(WINAPI*)(LPCTSTR, LPCTSTR, INSTALLSTATE);
    return reinterpret_cast<fun>(GETPROCADDR(MsiConfigureFeature))(
        szProduct, szFeature, eInstallState);
}


UINT C::msiGetProductInfo(
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
