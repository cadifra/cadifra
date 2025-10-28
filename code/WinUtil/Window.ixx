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


    LPCTSTR GetAtom() const { return MAKEINTATOM(itsAtom); }

private:
    ATOM itsAtom = {};
    HINSTANCE itsInstance = {};
};


export class WindowDisabler
{
public:
    WindowDisabler(); // Disables all visible top level windows of the calling thread

    WindowDisabler(const WindowDisabler&) = delete;
    WindowDisabler& operator=(const WindowDisabler&) = delete;

    ~WindowDisabler(); // calls Enable()

    void Enable(); // Reenables all previous disabled windows.

    static void AddAndDisableIfNeeded(HWND);

private:
    HWND itsOldActive;
    DWORD itsProcessId;

    using Windows = std::vector<HWND>;
    Windows itsWindows;

    using DisablerList = std::list<WindowDisabler*>;
    static DisablerList theDisablerList;

    DisablerList::iterator itsListPos;

    static BOOL CALLBACK EnumWindowsProcNoThrow(HWND hwnd, LPARAM lParam);
    static BOOL EnumWindowsProc(HWND hwnd, LPARAM lParam);

    static LRESULT CALLBACK SubClassWindowProcNoThrow(
        HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    static LRESULT SubClassWindowProc(
        HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};


export class WindowPlacement: public WINDOWPLACEMENT
{
public:
    WindowPlacement();

    // uses compiler generated copy-ctor and assignment-op
};


export auto CalcNewWindowPos(HWND w) -> WindowPlacement;

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
    ImpPtr imp;

public:
    WindowPlacementHandler();
    explicit WindowPlacementHandler(HWND w);
    explicit WindowPlacementHandler(const WindowPlacement& wp);

    WindowPlacementHandler(const WindowPlacementHandler&);
    WindowPlacementHandler& operator=(const WindowPlacementHandler&);

    ~WindowPlacementHandler();

    bool ok() const { return imp.get() != 0; }

    void ChangeShowCmd(UINT flags);

    void GetFrom(HWND w);
    void SetTo(HWND w) const;

    // Read and Write may throw a Registry::Exception
    void Read(const Registry::Key& k, const std::wstring& valueName);
    void Write(const Registry::Key& k, const std::wstring& valueName) const;
};


export LRESULT CALLBACK WindowStartupProc(HWND h, UINT uMsg, WPARAM wp, LPARAM lp);

// Use this function as the WNDPROC for windows you create with "CreateWindow"
// or "CreateWindowEx" with the "lpParam" set to a WinUtil::Window pointer.

// The WindowStartupProc will wait for the WM_CREATE message, extract the pointer
// to the WinUtil::Window and subclass the window.


export class ChildWindowVisitor
{
public:
    virtual bool Visit(HWND hwnd) = 0;

protected:
    ~ChildWindowVisitor() = default;
};

export void VisitChildWindows(HWND parent, ChildWindowVisitor* Visitor);
// For every child window of parent, the member function Visitor->Visit
// is called with the child window handle as parameter.
// The Visit member function has to return false if it wants to stop
// the visiting process.


// Useful example of a ChildWindowVisitor:
//
export class ChildWindowSender: public ChildWindowVisitor
{
    UINT itsMsg;
    WPARAM itsWParam;
    LPARAM itsLParam;
    bool Visit(HWND hwnd) override; // calls SendMessage for hwnd
public:
    ChildWindowSender(UINT uMsg, WPARAM wParam, LPARAM lParam);
};


}
