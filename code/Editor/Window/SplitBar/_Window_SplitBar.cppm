/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Editor.Window:SplitBar;

import :Window;


namespace Editor
{

class Window::SplitBar
{
    Window& owner_;
    bool moving_ = false;
    bool useKey_ = false;
    WinUtil::HBRUSH brush_ = {};
    WinUtil::Window window_;
    RECT dragRect_ = {};
    int splitPos_ = 0;
    bool itIsSplit = false;
    d1::Point oldCursorPos_;
    WinUtil::ProcRegistrar procReg_;

public:
    SplitBar(Window&);

    SplitBar(const SplitBar&) = delete;
    SplitBar& operator=(const SplitBar&) = delete;

    void create(HWND parent, const WinUtil::HINSTANCE& instance);

    HWND getWindowHandle() const { return window_.getWindowHandle(); }

    bool isSplit() const { return itIsSplit; }
    void setSplit(bool split) { itIsSplit = split; }

    int splitPos() const { return splitPos_; }
    void setSplitPos(int p) { splitPos_ = p; }

    void startSplitBarMove(int pos, bool useKey);
    void stopSplitBarMove(bool cancel);
    void drawSplitBarDragRect(int pos, bool start, bool stop);
    void removeSplitBar();

private:
    void onPaint(WinUtil::WM_PAINT_Msg);
    void onLButtonDown(WinUtil::WM_LBUTTONDOWN_Msg);
    void onLButtonUp(WinUtil::WM_LBUTTONUP_Msg);
    void onLButtonDbClk(WinUtil::WM_LBUTTONDBLCLK_Msg);
    void onMouseMove(WinUtil::WM_MOUSEMOVE_Msg);
    void onKillFocus(WinUtil::WM_KILLFOCUS_Msg);
    void onKeyDown(WinUtil::WM_KEYDOWN_Msg);
    void onCaptureChanged(WinUtil::WM_CAPTURECHANGED_Msg);
    void onSetCursor(WinUtil::WM_SETCURSOR_Msg);
};

}

