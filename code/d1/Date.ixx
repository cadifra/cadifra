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
    bool /*ok*/ convertFromISO(const std::string&);

    auto operator<=>(const Date& rhs) const = default;

    bool isValid() const;
    bool isLeapYear() const { return isLeapYear(year); }

    static bool isLeapYear(uint16 year);
};

}
