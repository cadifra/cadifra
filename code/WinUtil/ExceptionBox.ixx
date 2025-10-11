/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil:ExceptionBox;

import std;


export namespace WinUtil::ExceptionBox
{

void SetTitle(const std::string&);

std::streambuf& Clear();
// Clears the buffer and returns streambuf to fill the buffer with
// text. The size of the buffer is limited. If the buffer is full, all
// write operations on the returned ostream object are discarded.

void Show();
// Shows a message box with the content of the buffer.

}
