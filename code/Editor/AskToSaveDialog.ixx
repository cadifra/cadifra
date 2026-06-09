/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

#include "Resources/StringMacro.h"

export module Editor.AskToSaveDialog;

import d1.wintypes;

import std;


export namespace Editor::AskToSaveDialog
{

enum class Result
{
    No,
    Yes,
    Cancel
};

Result showDialog(d1::HWND owner, const std::wstring& fileName);

}

module : private;

import WinUtil.CursorManager;
import WinUtil.ResourceLoader;
import WinUtil.WindowDisabler;

import Resources;


D1_RESOURCES_USE_STRING(SaveChanges)
D1_RESOURCES_USE_STRING(AppName)


namespace Editor::AskToSaveDialog
{

Result showDialog(HWND owner, const std::wstring& fileName)
{
    auto iwc = WinUtil::CursorManager::InhibitWaitCursor{};
    auto wd = WinUtil::WindowDisabler{};

    auto list = WinUtil::ResourceLoader::StringListType{};
    list.push_back(fileName);

    const auto s = std::wstring{ WinUtil::ResourceLoader::instance()
            .getFormatString(Resources::Strings::SaveChanges.id, list) };

    const auto title = std::wstring{ Resources::Strings::AppName };

    switch (::MessageBox(owner, s.c_str(), title.c_str(),
        MB_YESNOCANCEL | MB_ICONWARNING | MB_SETFOREGROUND))
    {
        using enum Result;
    case IDYES:
        return Yes;
    case IDNO:
        return No;
    default:
        return Cancel;
    }
}

}
