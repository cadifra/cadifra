/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.Window;

import WinUtil.Registry;

import std;


namespace WinUtil
{

export class WindowClass
{
public:
    WindowClass(
        LPCTSTR lpszClassName,
        WNDPROC lpfnWndProc,
        HINSTANCE hInstance,
        UINT style = 0,
        HBRUSH hbrBackground = 0,
        HCURSOR hCursor = 0,
        LPCTSTR lpszMenuName = 0,
        HICON hIcon = 0,
        HICON hIconSm = 0,
        int cbClsExtra = 0,
        int cbWndExtra = 0);
    // calls RegisterClassEx

    WindowClass(const WindowClass&) = delete;
    WindowClass& operator=(const WindowClass&) = delete;

    ~WindowClass(); // intentionally not virtual
    // calls UnregisterClass


    LPCTSTR getAtom() const { return MAKEINTATOM(atom_); }

private:
    ATOM atom_ = {};
    HINSTANCE instance_ = {};
};


export class WindowDisabler
{
public:
    WindowDisabler(); // Disables all visible top level windows of the calling thread

    WindowDisabler(const WindowDisabler&) = delete;
    WindowDisabler& operator=(const WindowDisabler&) = delete;

    ~WindowDisabler(); // calls Enable()

    void enable(); // Reenables all previous disabled windows.

    static void addAndDisableIfNeeded(HWND);

private:
    HWND oldActive_;
    DWORD processId_;

    using Windows = std::vector<HWND>;
    Windows windows_;

    using DisablerList = std::list<WindowDisabler*>;
    static DisablerList theDisablerList;

    DisablerList::iterator listPos_;

    static BOOL CALLBACK enumWindowsProcNoThrow(HWND hwnd, LPARAM lParam);
    static BOOL enumWindowsProc(HWND hwnd, LPARAM lParam);

    static LRESULT CALLBACK subClassWindowProcNoThrow(
        HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    static LRESULT subClassWindowProc(
        HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};


export class WindowPlacement: public WINDOWPLACEMENT
{
public:
    WindowPlacement();

    // uses compiler generated copy-ctor and assignment-op
};


export auto calcNewWindowPos(HWND w) -> WindowPlacement;

// Calculates a reasonable window position for a new top level window
// relative to an existing top level window "w".

// The new window position is maximized/minimized, if "w" is maximized/
// minimized.

// The new window is "cascaded" on the window "w" to the left downwards.
// If there is no more room on the workspace (workspace = desktop without
// toolbars)to "cascade" the new window, the top left position of the
// new window is in the top left corner of the workspace.

// The size of the new window is always the same as the size of "w".

// The new window is always on the same monitor as "w".


export class WindowPlacementHandler
{
    using ImpPtr = std::unique_ptr<WindowPlacement>;
    ImpPtr imp_;

public:
    WindowPlacementHandler();
    explicit WindowPlacementHandler(HWND w);
    explicit WindowPlacementHandler(const WindowPlacement& wp);

    WindowPlacementHandler(const WindowPlacementHandler&);
    WindowPlacementHandler& operator=(const WindowPlacementHandler&);

    ~WindowPlacementHandler();

    bool ok() const { return imp_.get() != 0; }

    void changeShowCmd(UINT flags);

    void getFrom(HWND w);
    void setTo(HWND w) const;

    // Read and Write may throw a Registry::Exception
    void read(const Registry::Key& k, const std::wstring& valueName);
    void write(const Registry::Key& k, const std::wstring& valueName) const;
};


export LRESULT CALLBACK windowStartupProc(HWND h, UINT uMsg, WPARAM wp, LPARAM lp);

// Use this function as the WNDPROC for windows you create with "CreateWindow"
// or "CreateWindowEx" with the "lpParam" set to a WinUtil::Window pointer.

// The WindowStartupProc will wait for the WM_CREATE message, extract the pointer
// to the WinUtil::Window and subclass the window.


export class ChildWindowVisitor
{
public:
    virtual bool visit(HWND hwnd) = 0;

protected:
    ~ChildWindowVisitor() = default;
};

export void visitChildWindows(HWND parent, ChildWindowVisitor* Visitor);
// For every child window of parent, the member function Visitor->Visit
// is called with the child window handle as parameter.
// The Visit member function has to return false if it wants to stop
// the visiting process.


// Useful example of a ChildWindowVisitor:
//
export class ChildWindowSender: public ChildWindowVisitor
{
    UINT msg_;
    WPARAM WParam_;
    LPARAM LParam_;
    bool visit(HWND hwnd) override; // calls SendMessage for hwnd
public:
    ChildWindowSender(UINT uMsg, WPARAM wParam, LPARAM lParam);
};


}
