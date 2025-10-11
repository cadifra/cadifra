/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.Date;

import d1.types;

import std;


namespace d1
{

export struct Date
{
    uint16 year = 0, month = 0, day = 0;

    Date() {}
    bool /*ok*/ ConvertFromISO(const std::string&);

    bool operator<=(const Date& r) const;

    bool operator>(const Date& r) const
    {
        return !(*this <= r);
    }

    bool IsValid() const;
    bool IsLeapYear() const { return IsLeapYear(year); }

    static bool IsLeapYear(uint16 year);
};

}
