/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>
#include <msi.h>

export module WinUtil:MsiLib;


namespace WinUtil
{

export class MsiLib
{
    HMODULE itsHModule;

public:
    MsiLib();

    MsiLib(const MsiLib&) = delete;
    MsiLib& operator=(const MsiLib&) = delete;

    ~MsiLib();

    operator bool() const { return 0 != itsHModule; }

    UINT MsiProvideComponent(
        LPCTSTR szProduct,   // product code in case install required
        LPCTSTR szFeature,   // feature ID in case install required
        LPCTSTR szComponent, // component ID
        DWORD dwInstallMode, // the install mode
        LPTSTR lpPathBuf,    // returned path to component key file
        DWORD* pcchPathBuf   // in/out buffer character count
    ) const;

    INSTALLSTATE MsiGetComponentPath(
        LPCTSTR szProduct,   // product code for client product
        LPCTSTR szComponent, // component ID
        LPTSTR lpPathBuf,    // returned path
        DWORD* pcchBuf       // buffer character count
    ) const;

    INSTALLSTATE MsiQueryFeatureState(
        LPCTSTR szProduct,
        LPCTSTR szFeature) const;

    UINT MsiConfigureFeature(
        LPCTSTR szProduct,
        LPCTSTR szFeature,
        INSTALLSTATE eInstallState) const;

    UINT MsiGetProductInfo(
        LPCTSTR szProduct,
        LPCTSTR szProperty,
        LPTSTR lpValueBuf,
        DWORD* pcchValueBuf) const;
};


}
