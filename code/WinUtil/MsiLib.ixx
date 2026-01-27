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


}
