/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Editor.Internals;

import App.IApplication;
import App.ISdiApp;

import d1.wintypes;

import Store.IStorable;

import WinUtil.ProductVersion;

import std;


namespace Editor
{

export namespace AllFactories
{
void report(Store::IFactoryReporter& r);
// calls r.report(x) for every Store::IFactory "x".
}


export class GUIDs
{
public:
    static auto diagram() -> const d1::CLSID&;
};


export namespace ProductVersion
{

auto get() -> const WinUtil::ProductVersion&;

}


export void showAboutDialog(App::IApplication&, d1::HWND parent);


export namespace AskToSaveDialog
{

enum class Result
{
    No,
    Yes,
    Cancel
};

Result showDialog(d1::HWND owner, const std::wstring& fileName);

}


export namespace RegistrySettings
{

// User Settings:

extern const char* FrameWindowPositionVal;
extern const char* MaximizeMDIVal;

#pragma pack(push, 1)
struct WindowPosition
{
    d1::RECT normalPosition;
    bool isMaximized;
};
#pragma pack(pop)


}


export class EvalExpire
{
public:
    static auto instance() -> EvalExpire&; // singleton

    ~EvalExpire();

    int remainingDays() const;

    constexpr static int MaxDays = 30;

private:
    EvalExpire();

    class Imp;
    std::unique_ptr<Imp> imp_;
};


export class NewWindowDefaultsProvider:
    public App::NewWindowDefaultsProvider
{
    bool wtb_;

public:
    explicit NewWindowDefaultsProvider(bool wtb):
        wtb_{ wtb }
    {
    }
    bool withToolBar() const { return wtb_; }
};


}
