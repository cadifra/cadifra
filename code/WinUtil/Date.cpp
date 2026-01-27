/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil.Date;


namespace WinUtil::Date
{

d1::Date getLocalDate()
{
    auto res = d1::Date{};

    auto st = SYSTEMTIME{};
    ::GetLocalTime(&st);

    res.year = st.wYear;
    res.month = st.wMonth;
    res.day = st.wDay;

    return res;
}

}
