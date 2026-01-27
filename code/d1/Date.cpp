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

bool checkSeparator(std::istream& in, wchar_t sep = L'-')
{
    if (not in)
        return false;
    char c = 0;
    in.get(c);
    if (in.fail())
        return false;
    bool ok = (c == sep);
    if (not ok)
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
    while (not s.empty())
    {
        char c = s.at(0);
        s.erase(0, 1);
        if (c == '-')
            return count;
        if (not std::isdigit(c))
            throw DateConversionError{};
        ++count;
    }
    return count;
}


void checkFormat(const std::string in_s)
{
    auto s = in_s;

    if (digits(s) != 4)
        throw DateConversionError{};

    if (digits(s) != 2)
        throw DateConversionError{};

    if (digits(s) != 2)
        throw DateConversionError{};

    if (not s.empty())
        throw DateConversionError{};
}

}


bool /*ok*/ Date::convertFromISO(const std::string& s)
{
    const bool ok = true;

    Date res;

    if (s.empty())
        return not ok;

    try
    {
        checkFormat(s);
    }
    catch (DateConversionError&)
    {
        return not ok;
    }

    auto is = std::istringstream{ s };
    is.exceptions(std::ios_base::badbit | std::ios_base::failbit);

    try
    {
        is >> res.year;
        checkSeparator(is);
        is >> res.month;
        checkSeparator(is);
        is >> res.day;
    }
    catch (std::ios_base::failure&)
    {
        return not ok;
    }

    if (not is.eof())
        return not ok;

    if (not res.isValid())
        return not ok;

    *this = res;

    return ok;
}


bool Date::isValid() const
{
    const bool valid = true;

    if (year == 0 or month == 0 or day == 0)
        return not valid;

    if (month > 12)
        return not valid;

    if (day > 31)
        return not valid;

    if (day == 31 and (month == 4 or month == 6 or month == 9 or month == 11))
        return not valid;

    if (month != 2)
        return valid;

    D1_ASSERT(month == 2);

    if (day > 29)
        return not valid;

    if (day < 29)
        return valid;

    D1_ASSERT(day == 29);

    if (not isLeapYear())
        return not valid;

    return valid;
}


bool Date::isLeapYear(uint16 year)
{
    const bool LeapYear = true;

    if (year % 4 != 0)
        return not LeapYear;

    /* year is divisible by four */

    if (year % 400 == 0)
        return LeapYear;

    if (year % 100 == 0)
        return not LeapYear;

    return LeapYear;
}

}

