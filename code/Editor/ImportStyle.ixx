/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "Resources/StringMacro.h"

#include <Windows.h>

export module Editor.ImportStyle;

import Store.IStorable;

import View.Base;


namespace Editor
{

export void importStyle(
    const Store::FactoryRegistry& fr, const View::TaskEnv& te);

}

module : private;


import Editor.StorageHolder;
import Editor.ProductVersion;

import Core.Grid;
import Core.Main;

import Style.Main;
import Style.ImportStyle;

import WinUtil.OpenFileDialog;

import Diagram;
import Resources;


D1_RESOURCES_USE_STRING(DiagramFileFilter)
D1_RESOURCES_USE_STRING(ImportStyleDialogTitle)


namespace Editor
{

void importStyle(
    const Store::FactoryRegistry& fr, const View::TaskEnv& te)
{
    const HWND window = ::GetActiveWindow();

    using OFD = WinUtil::OpenFileDialog;

    OFD::Flags flags;

    flags.AllowMultiselect = false;
    flags.HideReadOnly = true;

    auto title = std::wstring{ Resources::Strings::ImportStyleDialogTitle };

    auto dlg = OFD{ window, Resources::Strings::DiagramFileFilter, &flags, &title };

    if (not dlg.isOk)
        return;


    auto fileName = *begin(dlg.fileList);


    struct NullInitializer: public ::Diagram::Diagram::Initializer
    {
        void initializeNewDiagram(::Diagram::Diagram&) {}
    } init;

    struct NullPastePostProcessor: public Core::IPastePostProcessor
    {
        void pastePostProcessing(Core::Env&, const Core::ElementSet&) const {}
    } ppp;

    auto diag = ::Diagram::Diagram{
        fr, init, ppp, ProductVersion::get(),
        Core::IGrid::get(), te.p.document.getApplication()
    };

    auto sh = StorageHolder{ diag };

    sh.open(fileName, /*readOnly*/ true);

    Style::importStyle(te, diag);
}

}
