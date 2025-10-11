/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module WinUtil;


namespace WinUtil
{

bool ProductVersion::operator>(const ProductVersion& rhs) const
{
    if (first > rhs.first)
        return true;
    else if (first == rhs.first)
    {
        if (second > rhs.second)
            return true;
        else if (second == rhs.second)
        {
            if (third > rhs.third)
                return true;
        }
    }

    return false;
}

}

