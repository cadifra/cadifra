/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

#include "Resources/StringMacro.h"

export module Editor.EditorApp;

import Editor.Diagram;

import App.ISdiApp;
import App.IDocument;
import App.Clipboard;

import std;


namespace Editor
{

export class EditorApp:
    public App::ISdiApp,
    private App::IDocument::IFactory::Creator
{
public:
    EditorApp();

    EditorApp(const EditorApp&) = delete;
    EditorApp& operator=(const EditorApp&) = delete;

    ~EditorApp();

private:
    //-- App::ISdiApp

    auto createDocumentFactory(App::LifeTimeSupervisor&)
        -> App::IDocument::IFactory& override;

    int init();

    //-- App::CommandLine::IReceiver

    void interpretCommandLine(const std::wstring&) override;

    //-- App::IDocumentFactoryCreator

    auto getApplication() -> App::IApplication& override;

    //--

    std::unique_ptr<Diagram::Factory> factory_;
    std::unique_ptr<App::IClipboard> clipboard_;

    void interpretCommandLine(std::wistream& is);
    void openFiles(std::wistream& is);
    void executeOption(std::wistream& is);
    void print(std::wistream& is);
    void printto(std::wistream& is);
};

}

module : private;

import Editor.Installer;
import Editor.NewWindowPlacement;

import App.ContextMenu;

import d1.buffer;
import d1.CaseInsensitive;
import d1.stringConvert;

import WinUtil.Debug;
import WinUtil.ExceptionBox;
import WinUtil.ResolveLink;

import Page.SelectedPrinter;

import Resources;

import std;


D1_RESOURCES_USE_STRING(AppName)
D1_RESOURCES_USE_STRING(ErrorBoxTitle)
D1_RESOURCES_USE_STRING(DiagramDefaultExt)


namespace Editor
{

const wchar_t* ContextMenuRegistryName = L"ContextMenuDefaults";


#ifdef _DEBUG
bool debugOutputEnabled()
{
    return (WinUtil::DebugEnv::inst().getInt(
                "Editor", "Editor::InterpretCommandLine") != 0);
}
#endif


using C = EditorApp;


C::EditorApp():
    clipboard_{ std::make_unique<App::Clipboard>() }
{
    NewWindowPlacement::initFromRegistry();
    try
    {
        App::ContextMenu::readDefaults(
            Installer::instance().getUserSettingsKey(false), ContextMenuRegistryName);
    }
    catch (WinUtil::Registry::Exception)
    {
    }
}


C::~EditorApp()
{
    NewWindowPlacement::storeToRegistry();
    try
    {
        App::ContextMenu::writeDefaults(
            Installer::instance().getUserSettingsKey(true), ContextMenuRegistryName);
    }
    catch (WinUtil::Registry::Exception)
    {
    }
}


void consumeSpace(std::wistream& is)
{
    while (is and iswspace(is.peek()))
        is.ignore();
}


std::wstring readString(std::wistream& is)
{
    consumeSpace(is);

    if (is.peek() == L'"')
    {
        is.ignore();
        std::wstring s;

        auto c = is.get();
        constexpr auto eof = std::wistringstream::traits_type::eof();

        for (; c != L'"' and c != eof; c = is.get())
            s.push_back(c);

        return s;
    }
    else
    {
        std::wstring s;
        is >> s;
        return s;
    }
}



std::wstring readExistingFileName(std::wistream& is)
{
    auto s = std::wstring{ readString(is) };

    if (s.empty())
        return s;

    const auto defExt = std::wstring(L"." + Resources::Strings::DiagramDefaultExt.get());

    auto buf = d1::wbuffer(MAX_PATH + 1);

    LPTSTR fp = 0;
    DWORD res = ::SearchPath(
        L".\\", s.c_str(), defExt.c_str(),
        static_cast<DWORD>(buf.size() - 1),
        buf.data(), &fp);

    if (not res)
        return s;

    if (res >= buf.size() - 1)
    {
        buf.resize(res + 10, 0);
        res = ::SearchPath(
            0, s.c_str(), defExt.c_str(),
            static_cast<DWORD>(buf.size() - 1),
            buf.data(), &fp);
    }

    buf.resize(wcslen(buf.c_str()));

    if (not res)
        return s;

    return WinUtil::resolveLink(buf, 0);
}


auto extractFilenames(std::wistream& is) -> std::vector<std::wstring>
{
    auto fileNames = std::vector<std::wstring>{};

    while (is)
    {
        auto s = readExistingFileName(is);

        if (not s.empty())
            fileNames.push_back(s);
    }

    return fileNames;
}


void C::openFiles(std::wistream& is)
{
    if (not factory_)
        return;

    for (auto& n : extractFilenames(is))
        factory_->openFile(n, false);
}


void C::print(std::wistream& is)
{
    auto s = readExistingFileName(is);

    if (not factory_ or s.empty())
        return;

    Diagram* d = factory_->testForReopen(s);

    const bool alreadyOpen = d != 0;

    if (not d)
        d = factory_->openFile(s, false);

    if (not d)
        return;

    d->print(Page::SelectedPrinter::instance().getName(), L"", L"");

    if (not alreadyOpen)
        d->userClose();
}


void C::printto(std::wistream& is)
{
    auto s = readExistingFileName(is);

    auto printer = readString(is);
    auto driver = readString(is);
    auto output = readString(is);

    if (not factory_ or s.empty())
        return;

    Diagram* d = factory_->testForReopen(s);

    const bool alreadyOpen = d != 0;

    if (not d)
        d = factory_->openFile(s, false);

    if (not d)
        return;

    if (printer.empty())
        d->print(Page::SelectedPrinter::instance().getName(), L"", L"");
    else
        d->print(printer, driver, output);

    if (not alreadyOpen)
        d->userClose();
}


void C::executeOption(std::wistream& is)
{
    constexpr auto eof = std::wistringstream::traits_type::eof();
    std::wstring s;

    while (is and not iswspace(is.peek()) and is.peek() != L'"' and is.peek() != eof)
        s.push_back(is.get());

    if (d1::CaseInsensitiveEqual<>{}(s, L"embedding") or
        d1::CaseInsensitiveEqual<>{}(s, L"embed"))
        ; // do nothing
    else if (s == L"p")
        print(is);
    else if (s == L"pt")
        printto(is);
    else if (factory_)
        factory_->newDiagram(0);
}


void C::interpretCommandLine(std::wistream& is)
{
    consumeSpace(is);

    if (is.peek() == L'-' or is.peek() == L'/')
    {
        is.ignore();
        executeOption(is);
    }
    else if (is)
        openFiles(is);
    else if (factory_)
        factory_->newDiagram(0);
}


void C::interpretCommandLine(const std::wstring& cmdline)
{
#ifdef _DEBUG
    if (debugOutputEnabled())
        WinUtil::dout
            << "Editor::interpretCommandLine("
            << d1::wstring2string(cmdline) << ")" << std::endl;
#endif

    auto is = std::wistringstream{ cmdline };

    interpretCommandLine(is);
}


auto C::createDocumentFactory(App::LifeTimeSupervisor& ls)
    -> App::IDocument::IFactory&
{
    using FC = Diagram::Factory::Creator*;

    factory_ = std::make_unique<Diagram::Factory>(
        *FC{ this }, ls, *clipboard_, getInstanceHandle());

    return *factory_;
}


int C::init()
{
    WinUtil::ExceptionBox::setTitle(
        d1::wstring2string(Resources::Strings::ErrorBoxTitle));

    return 0;
}


auto C::getApplication() -> App::IApplication&
{
    return *this;
}

}
