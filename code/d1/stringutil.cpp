/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module d1.stringutil;

namespace d1
{

int replace(
    std::wstring& in, const std::wstring& what, const std::wstring& with)
{
    using S = std::wstring;
    S s;
    S::size_type p0 = 0;
    S::size_type p1 = 0;
    int count = 0;
    for (;;)
    {
        p1 = in.find(what, p0);
        if (p1 == S::npos)
        {
            s += in.substr(p0);
            s.swap(in);
            return count;
        }
        ++count;
        s += in.substr(p0, p1 - p0);
        s += with;
        p0 = p1 + what.size();
    }
    s.swap(in);
    return count;
}


}
