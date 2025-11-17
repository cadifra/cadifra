/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1assert.h"

module d1.Date;

import std;


namespace d1
{


namespace
{

bool CheckSeparator(std::istream& in, wchar_t sep = L'-')
{
    if (!in)
        return false;
    char c = 0;
    in.get(c);
    if (in.fail())
        return false;
    bool ok = (c == sep);
    if (!ok)
        in.setstate(std::ios_base::failbit);
    return ok;
}

}


namespace
{

class DateConversionError: public std::exception
{
public:
    const char* what() const { return "DateConversionError"; }
};


auto digits(std::string& s) -> std::string::size_type
{
    std::string::size_type count = 0;
    while (!s.empty())
    {
        char c = s.at(0);
        s.erase(0, 1);
        if (c == '-')
            return count;
        if (!std::isdigit(c))
            throw DateConversionError{};
        ++count;
    }
    return count;
}


void CheckFormat(const std::string in_s)
{
    auto s = in_s;

    if (digits(s) != 4)
        throw DateConversionError{};

    if (digits(s) != 2)
        throw DateConversionError{};

    if (digits(s) != 2)
        throw DateConversionError{};

    if (!s.empty())
        throw DateConversionError{};
}

}


bool /*ok*/ Date::ConvertFromISO(const std::string& s)
{
    const bool ok = true;

    Date res;

    if (s.empty())
        return !ok;

    try
    {
        CheckFormat(s);
    }
    catch (DateConversionError&)
    {
        return !ok;
    }

    auto is = std::istringstream{ s };
    is.exceptions(std::ios_base::badbit | std::ios_base::failbit);

    try
    {
        is >> res.year;
        CheckSeparator(is);
        is >> res.month;
        CheckSeparator(is);
        is >> res.day;
    }
    catch (std::ios_base::failure&)
    {
        return !ok;
    }

    if (!is.eof())
        return !ok;

    if (!res.IsValid())
        return !ok;

    *this = res;

    return ok;
}


bool Date::operator<=(const Date& r) const
{
    if (year < r.year)
        return true;
    if (year != r.year)
        return false;
    if (month < r.month)
        return true;
    if (month != r.month)
        return false;
    return day <= r.day;
}


bool Date::IsValid() const
{
    const bool valid = true;

    if (year == 0 || month == 0 || day == 0)
        return !valid;

    if (month > 12)
        return !valid;

    if (day > 31)
        return !valid;

    if (day == 31 && (month == 4 || month == 6 || month == 9 || month == 11))
        return !valid;

    if (month != 2)
        return valid;

    D1_ASSERT(month == 2);

    if (day > 29)
        return !valid;

    if (day < 29)
        return valid;

    D1_ASSERT(day == 29);

    if (!IsLeapYear())
        return !valid;

    return valid;
}


bool Date::IsLeapYear(uint16 year)
{
    const bool LeapYear = true;

    if (year % 4 != 0)
        return !LeapYear;

    /* year is divisible by four */

    if (year % 400 == 0)
        return LeapYear;

    if (year % 100 == 0)
        return !LeapYear;

    return LeapYear;
}

}

