/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module App.DialogLayout;


export namespace App::DialogLayout
{

class DlgItem
{
    HWND dlg_;

public:
    DlgItem(HWND d);
    HWND operator()(int id) const;
};


LONG width(const RECT& r);
LONG height(const RECT& r);
SIZE getSize(const RECT& r); // Returns the width and height of r
SIZE makeSize(LONG x, LONG y);


SIZE convertDialogUnits(HWND w, const SIZE& s);
// Converts the dialog units s in pixel coordinates


RECT resizeClientRect(HWND w, const SIZE& s);
// Enlarges the window w such that its client rect equals s.
// The upper left coordinate of w is not changed.
// Returns the new window rect of w in client coordinates of
// the parent of w.


enum class Corner
{
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};


SIZE moveInside(HWND t, Corner ct, HWND r, Corner cr, const SIZE& d);
// Moves the window "t" such that its window rect corner "ct"
// gets the distance "d" to the client rect corner "cr" of "r".
// Returns the offset, the window "t" was moved.


SIZE moveAbsolute(HWND t, Corner ct, const RECT& r, Corner cr, const SIZE& d);
// Moves the window "t" such that its window rect corner "ct" gets the
// distance "d" to the corner "cr" of the RECT "r" ("r" in screen coordinates).
// Returns the offset, the window "t" was moved.


void moveDelta(HWND t, const SIZE& d);
// Moves the window "t" "d.cx" pixels to the right and "d.cy" pixels to the bottom


void defaultPlace(HWND dlg);
// Tries to place dlg's window nicely.

}

module : private;


namespace App::DialogLayout
{

namespace
{

HWND parent(HWND w)
{ // returns 0 if w has no parent
    return ::GetParent(w);
}

void mapWindowRect(HWND from, HWND to, RECT& r)
{
    D1_VERIFY(::MapWindowPoints(from, to, reinterpret_cast<POINT*>(&r), 2));
}

void mapWindowPoint(HWND from, HWND to, POINT& p)
{
    D1_VERIFY(::MapWindowPoints(from, to, &p, 1));
}

POINT getCorner(const RECT& r, Corner cr)
{
    auto p = POINT{ 0, 0 };
    switch (cr)
    {
        using enum Corner;
    case TopLeft:
        p.x = r.left;
        p.y = r.top;
        break;
    case TopRight:
        p.x = r.right;
        p.y = r.top;
        break;
    case BottomLeft:
        p.x = r.left;
        p.y = r.bottom;
        break;
    case BottomRight:
        p.x = r.right;
        p.y = r.bottom;
        break;
    }
    return p;
}

}


DlgItem::DlgItem(HWND d):
    dlg_{ d }
{
    D1_ASSERT(dlg_);
}

HWND DlgItem::operator()(int id) const
{
    HWND r = ::GetDlgItem(dlg_, id);
    D1_ASSERT(r);
    return r;
}


LONG width(const RECT& r)
{
    return r.right - r.left;
}


LONG height(const RECT& r)
{
    return r.bottom - r.top;
}


SIZE getSize(const RECT& r)
{
    return { width(r), height(r) };
}


SIZE makeSize(LONG x, LONG y)
{
    return { x, y };
}


SIZE convertDialogUnits(HWND w, const SIZE& s)
{
    auto r = RECT{ 0, 0, s.cx, s.cy };
    D1_VERIFY(::MapDialogRect(w, &r));
    return makeSize(width(r), height(r));
}


RECT resizeClientRect(HWND w, const SIZE& s)
{
    RECT actClient;
    RECT actWindow;

    D1_VERIFY(::GetClientRect(w, &actClient));
    D1_VERIFY(::GetWindowRect(w, &actWindow));

    auto newWindow = RECT{
        actWindow.left,
        actWindow.top,
        actWindow.right + s.cx - width(actClient),
        actWindow.bottom + s.cy - height(actClient)
    };

    mapWindowRect(0, parent(w), newWindow);

    D1_VERIFY(::SetWindowPos(
        w,
        0, /* hWndInsertAfter not used */
        newWindow.left,
        newWindow.top,
        width(newWindow),
        height(newWindow),
        SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER));

    return newWindow;
}


SIZE moveInside(HWND t, Corner ct, HWND r, Corner cr, const SIZE& d)
{
    RECT client;
    D1_VERIFY(::GetClientRect(r, &client));
    mapWindowRect(r, 0, client);
    return moveAbsolute(t, ct, client, cr, d);
}


SIZE moveAbsolute(HWND t, Corner ct, const RECT& r, Corner cr, const SIZE& d)
{
    RECT tw;
    D1_VERIFY(::GetWindowRect(t, &tw));

    POINT pt = getCorner(tw, ct);

    POINT pr = getCorner(r, cr);
    pr.x += d.cx;
    pr.y += d.cy;

    auto offset = SIZE{ pr.x - pt.x, pr.y - pt.y };
    moveDelta(t, offset);

    return offset;
}


void moveDelta(HWND t, const SIZE& d)
{
    RECT r;
    D1_VERIFY(::GetWindowRect(t, &r));

    r.left += d.cx;
    r.top += d.cy;

    mapWindowRect(0, parent(t), r);

    D1_VERIFY(::SetWindowPos(
        t,
        0, /* hWndInsertAfter not used */
        r.left,
        r.top,
        0, /* width not used */
        0, /* height not used */
        SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER));
}


void defaultPlace(HWND hwndDlg)
{
    // copied and adapted from MSDN

    HWND hwndOwner = ::GetParent(hwndDlg);

    if (hwndOwner == NULL)
        hwndOwner = ::GetDesktopWindow();

    RECT rc, rcDlg, rcOwner;

    ::GetWindowRect(hwndOwner, &rcOwner);
    ::GetWindowRect(hwndDlg, &rcDlg);
    ::CopyRect(&rc, &rcOwner);

    // Offset the owner and dialog box rectangles so that
    // right and bottom values represent the width and
    // height, and then offset the owner again to discard
    // space taken up by the dialog box.

    ::OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
    ::OffsetRect(&rc, -rc.left, -rc.top);
    ::OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

    // The new position is the sum of half the remaining
    // space and the owner's original position.

    ::SetWindowPos(
        hwndDlg,
        HWND_TOP,
        rcOwner.left + (rc.right / 2),
        rcOwner.top + (rc.bottom / 2),
        0, 0, // ignores size arguments
        SWP_NOSIZE);
}

}
