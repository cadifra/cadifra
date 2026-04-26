/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Editor.Util;

import App.Main;

import Store.IStorable;

import View.Base;

import std;


namespace Editor
{

export void importStyle(
    const Store::FactoryRegistry& fr, const View::TaskEnv& te);


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
