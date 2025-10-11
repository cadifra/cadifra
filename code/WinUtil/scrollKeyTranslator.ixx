/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil:scrollKeyTranslator;

import d1.wintypes;

import :Messages;


namespace WinUtil
{

export void scrollKeyTranslator(
    d1::HWND receiver,
    d1::HWND hScrollBar,
    d1::HWND vScrollBar,
    const WM_KEYDOWN_Msg& msg);

}
