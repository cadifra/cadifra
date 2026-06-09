/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "Resources/StringMacro.h"

#include "main/build-config.h"
#include "main/installer-config.h"
#include "main/LGUID.h"

#include <Windows.h>
#include <msi.h>

export module Editor.Installer;

import App.RegistryAccessProvider;

import std;


namespace Editor
{

export class Installer: public App::IRegistryAccessProvider
{
public:
    //-- App::IRegistryAccessProvider

    auto getUserSettingsKeyForReading() const -> WinUtil::Registry::Key override;
    auto getUserSettingsKeyForWriting() const -> WinUtil::Registry::Key override;

    //--

    static auto instance() -> Installer&; // singleton

    ~Installer();

    auto getUserSettingsKey(bool writeAccess = false) const -> WinUtil::Registry::Key;
    // may throw WinUtil::Registry::Exception

    std::wstring getHelpFilePath() const;

    std::wstring getInstallDate() const;

private:
    Installer();

    class Imp;
    std::unique_ptr<Imp> imp_;

    Installer(const Installer&) = delete;
    Installer& operator=(const Installer&) = delete;
};

}


module : private;

import d1.buffer;

import WinUtil.MsgBox;
import WinUtil.MsiLib;

import Resources;


D1_RESOURCES_USE_STRING(AppName)
D1_RESOURCES_USE_STRING(NoMSI)
D1_RESOURCES_USE_STRING(NotInstalled)


namespace Editor
{


auto ProductCode = D1_MAIN_LGUID(D1_MAIN_BUILD_CONFIG_PRODUCT_ID);

auto FeatureCadifra = L"Cadifra";

auto FeatureUserSettings = L"UserSettings";

auto ComponentUserSettings =
    D1_MAIN_LGUID(D1_MAIN_INSTALLER_CONFIG_COMPONENT_USER_SETTINGS_GUID);

auto ComponentCadifra =
    D1_MAIN_LGUID(D1_MAIN_INSTALLER_CONFIG_COMPONENT_CADIFRA_GUID);

auto ComponentHelp =
    D1_MAIN_LGUID(D1_MAIN_INSTALLER_CONFIG_COMPONENT_HELP_GUID);


using C = Installer;


class C::Imp
{
    std::wstring userSettingsSubKey_;
    HKEY userSettingsKey_;

public:
    Imp();
    WinUtil::Registry::Key getUserSettingsKey(bool writeAccess) const;

    std::wstring getHelpFilePath() const;
    std::wstring getInstallDate() const;
};


std::wstring getComponentPath(WinUtil::MsiLib& lib, const wchar_t* component)
{
    DWORD bufSize = 0;

    INSTALLSTATE is = lib.msiGetComponentPath(ProductCode, component, 0, &bufSize);

    if (is != INSTALLSTATE_LOCAL and is != INSTALLSTATE_SOURCE)
        return L"";

    auto buf = d1::wbuffer(bufSize + 1);
    bufSize = static_cast<DWORD>(buf.size());

    is = lib.msiGetComponentPath(ProductCode, component, buf.data(), &bufSize);

    buf.resize(bufSize);

    if (is != INSTALLSTATE_LOCAL and is != INSTALLSTATE_SOURCE)
        return L"";

    return buf;
}


C::Imp::Imp():
    userSettingsKey_{ HKEY_CURRENT_USER }
{
    auto lib = WinUtil::MsiLib{};

    if (not lib)
    {
        WinUtil::MsgBox::show(
            Resources::Strings::AppName, Resources::Strings::NoMSI);
        return;
    }

    // Make a simple install check for FeatureCadifra and increment the usage count
    // of this feature.
    UINT res = lib.msiProvideComponent(
        ProductCode, FeatureCadifra, ComponentCadifra, INSTALLMODE_DEFAULT, 0, 0);

    // The first time the user starts Cadifra, the UserSettings feature is only
    // advertised. Install it without producing warnings in the event log:
    INSTALLSTATE s = lib.msiQueryFeatureState(ProductCode, FeatureUserSettings);

    if (s == INSTALLSTATE_ADVERTISED)
        lib.msiConfigureFeature(ProductCode, FeatureUserSettings, INSTALLSTATE_LOCAL);

    // Make a simple install check for the UserSettings feature and increment
    // its usage count. Any needed reinstallations will write warnings into the
    // event log.
    UINT res2 = lib.msiProvideComponent(
        ProductCode, FeatureUserSettings, ComponentUserSettings, INSTALLMODE_DEFAULT, 0, 0);

    const auto us = getComponentPath(lib, ComponentUserSettings);

    if (not us.empty())
    {
        userSettingsSubKey_ = us.substr(4);

        const auto s = us.substr(0, 2);

        if (s == L"00")
            userSettingsKey_ = HKEY_CLASSES_ROOT;
        else if (s == L"01")
            userSettingsKey_ = HKEY_CURRENT_USER;
        else if (s == L"02")
            userSettingsKey_ = HKEY_LOCAL_MACHINE;
        else if (s == L"03")
            userSettingsKey_ = HKEY_USERS;
    }

    if (res != ERROR_SUCCESS or res2 != ERROR_SUCCESS or us.empty())
    {
        WinUtil::MsgBox::show(
            Resources::Strings::AppName, Resources::Strings::NotInstalled);
    }
}


auto C::Imp::getUserSettingsKey(bool writeAccess) const
    -> WinUtil::Registry::Key
{
    if (not userSettingsSubKey_.size())
        throw WinUtil::Registry::Exception{ ERROR_INSTALL_FAILURE };

    return WinUtil::Registry::open(userSettingsKey_, userSettingsSubKey_,
        writeAccess ? KEY_WRITE | KEY_QUERY_VALUE : KEY_QUERY_VALUE);
}


std::wstring C::Imp::getHelpFilePath() const
{
    auto lib = WinUtil::MsiLib{};
    if (not lib)
    {
        WinUtil::MsgBox::show(
            Resources::Strings::AppName, Resources::Strings::NoMSI);
        return std::wstring{};
    }

    UINT res = lib.msiProvideComponent(
        ProductCode,
        FeatureCadifra,
        ComponentHelp,
        INSTALLMODE_DEFAULT,
        0,
        0);

    return getComponentPath(lib, ComponentHelp);
}


std::wstring C::Imp::getInstallDate() const
{
    std::wstring date;

    auto lib = WinUtil::MsiLib{};
    if (not lib)
        date;

    auto buf = std::vector<TCHAR>(32, '\0');
    DWORD size = static_cast<DWORD>(buf.size());

    UINT res = lib.msiGetProductInfo(
        ProductCode,
        INSTALLPROPERTY_INSTALLDATE,
        buf.data(),
        &size);

    if (res != S_OK)
        return date;

    date.assign(buf.data(), size);

    return date;
}


C::Installer():
    imp_{ std::make_unique<Imp>() }
{
}


C::~Installer()
{
}


auto C::instance() -> Installer&
{
    static Installer inst;
    return inst;
}


auto C::getUserSettingsKey(bool writeAccess) const
    -> WinUtil::Registry::Key
{
    return imp_->getUserSettingsKey(writeAccess);
}


auto C::getUserSettingsKeyForReading() const
    -> WinUtil::Registry::Key
{
    return getUserSettingsKey(false);
}


auto C::getUserSettingsKeyForWriting()
    const -> WinUtil::Registry::Key
{
    return getUserSettingsKey(true);
}


std::wstring C::getHelpFilePath() const
{
    return imp_->getHelpFilePath();
}


std::wstring C::getInstallDate() const
{
    return imp_->getInstallDate();
}

}
