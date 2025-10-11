/*
 *     Copyright (c) 1998-2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.buffer;

import std;


namespace d1
{

export inline auto wbuffer(std::wstring::size_type size)
{
	// Initialize a wstring with given size, initialized to all 0.
	// 
	// Note: using std::wstring{ size, 0 } would be wrong, as
	// that would try to use the constructor from
	// std::initializer_list, which would create a wstring with size 2.
	// But luckily that wouldn't compile anyway, because size cannot
	// be converted (would require narrowing conversion).
	//
    return std::wstring(size, 0);
}

}
