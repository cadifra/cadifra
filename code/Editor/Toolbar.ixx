/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>
#include <commctrl.h>

export module Editor.Toolbar;

import Editor.IWindow;
import Editor.NewWindowDefaultsProvider;

import App.ICmdView;

import d1.Point;

import View.DiagramType;

import WinUtil.types;
import WinUtil.Messages;

import std;


namespace Editor
{

export class Toolbar
{
    class MessagePreProcessor;

    IWindow& owner_;
    WinUtil::HINSTANCE instance_ = {};
    HWND parent_ = {};

    HWND toolBarWindow_ = {};

    HWND diagramTypeControl_ = {};
    int diagramTypeControlCurrentIndex_ = 0;
    int numOfDiagramTypes_ = 0;

    HWND zoomControl_ = {};
    HWND zoomControlEditCtrl_ = {};
    bool isChangeFactorCallActive = false;
    int lastShownZoomFactor_ = 0;

    int height_ = 0;
    bool visible_ = false;

    std::unique_ptr<MessagePreProcessor> messagePreProcessor_;

public:
    static const wchar_t* showRegistryName()
    {
        return L"ShowToolbar";
    }

    Toolbar(IWindow& owner);

    Toolbar(const Toolbar&) = delete;
    Toolbar& operator=(const Toolbar&) = delete;

    ~Toolbar();

    void init(
        HWND parent, WinUtil::HINSTANCE instance,
        const NewWindowDefaultsProvider* dp);

    LONG height() const;
    LONG yoffset() const;

    void show(bool show);
    bool visible() const { return visible_; }
    HWND window() const { return toolBarWindow_; }

    void saveStateToRegistry() const;

    void notify(WinUtil::WM_COMMAND_Msg msg);

    void setDiagramType(View::DiagramTypeValue::T);

    void updateToFactor();

private:
    void calculateAndCacheHeight();

    void show();
    void hide();

    void restoreStateFromRegistry();

    void addButtons();

    void createEntries();
    void addEntry(int id);

    void createDiagramTypeComboBox();
    void createZoomComboBox();

    void notifyDiagramType(WinUtil::WM_COMMAND_Msg msg);
    void notifyZoom(WinUtil::WM_COMMAND_Msg msg);

    bool preProcess(MSG& msg);

    void applyFactor();
    d1::Point center() const;

    void setZoomFactor(int new_factor);
    void showZoomFactor(int factor);
    void postShowZoomFactor(int factor);

    static unsigned int showZoomFactorMessageID();

    void setFocusToOwner();

    void restoreLastShownZoomFactor();
};


class ToolbarEntry: public App::ICmdView
{
public:
    ToolbarEntry(Toolbar& tb, int id):
        ICmdView{ id }, toolbar_{ tb }
    {
    }

    ToolbarEntry(const ToolbarEntry&) = delete;
    ToolbarEntry& operator=(const ToolbarEntry&) = delete;

private:
    void update(bool isEnabled, App::CmdState) const override;

    Toolbar& toolbar_;
};


void ToolbarEntry::update(bool isEnabled, App::CmdState s) const
{
    const HWND w = toolbar_.window();
    D1_ASSERT(w);
    D1_VERIFY(
        ::SendMessage(
            w, TB_ENABLEBUTTON,
            (WPARAM)getId(), (LPARAM)isEnabled));
}

}
