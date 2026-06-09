/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.IDumpable;

import d1.UTF8cvt;

import std;

namespace Core
{

export class IDumpable
{
public:
    virtual void dump(std::wostream&) const = 0;

protected:
    ~IDumpable() = default;
};


export auto operator<<(std::wostream& s, const IDumpable& d) -> std::wostream&
{
    d.dump(s);
    return s;
}

export auto operator<<(std::ostream& s, const IDumpable& d) -> std::ostream&
{
#ifdef _DEBUG
    std::wstringstream w;
    d.dump(w);
    s << d1::convertToUTF8(w.str());
#endif
    return s;
}

}
