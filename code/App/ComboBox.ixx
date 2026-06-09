/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module App.ComboBox;

import std;


export namespace App::ComboBox
{

std::wstring getSelectedString(HWND wnd);

void addString(HWND wnd, const wchar_t* s);

void insertString(HWND wnd, const wchar_t* s, int pos = -1);

void selectString(HWND wnd, const wchar_t* s);

void resetContent(HWND wnd);

}


module : private;

import d1.buffer;


namespace App::ComboBox
{

std::wstring getSelectedString(HWND wnd)
{
    LRESULT sel = ::SendMessage(wnd, CB_GETCURSEL, 0, 0);

    if (sel == CB_ERR)
        return L"";

    LRESULT len = ::SendMessage(wnd, CB_GETLBTEXTLEN, sel, 0);

    if (len == CB_ERR)
        return L"";

    auto s = d1::wbuffer(len + 1);

    len = ::SendMessage(wnd, CB_GETLBTEXT, sel, reinterpret_cast<LPARAM>(s.data()));

    if (len == CB_ERR)
        return L"";

    s.resize(len);

    return s;
}


void addString(HWND wnd, const wchar_t* s)
{
    D1_VERIFY(CB_OKAY <= ::SendMessage(
                             wnd, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(s)));
}


void insertString(HWND wnd, const wchar_t* s, int pos)
{
    D1_VERIFY(CB_OKAY <= ::SendMessage(
                             wnd, CB_INSERTSTRING, pos, reinterpret_cast<LPARAM>(s)));
}


void selectString(HWND wnd, const wchar_t* s)
{
    ::SendMessage(wnd, CB_SELECTSTRING, -1, reinterpret_cast<LPARAM>(s));
}


void resetContent(HWND wnd)
{
    ::SendMessage(wnd, CB_RESETCONTENT, 0, 0);
}

}
