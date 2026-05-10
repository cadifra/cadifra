/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module App.ComboBox;

import std;


namespace App
{

export namespace ComboBox
{

std::wstring getSelectedString(HWND wnd);

void addString(HWND wnd, const wchar_t* s);

void insertString(HWND wnd, const wchar_t* s, int pos = -1);

void selectString(HWND wnd, const wchar_t* s);

void resetContent(HWND wnd);

}

}
