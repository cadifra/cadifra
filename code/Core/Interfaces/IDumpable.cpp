/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;
import :Interfaces;

import d1.UTF8cvt;


namespace Core
{

std::ostream& operator<<(std::ostream& s, const IDumpable& d)
{
#ifdef _DEBUG
    std::wstringstream w;
    d.dump(w);
    s << d1::convertToUTF8(w.str());
#endif
    return s;
}

}
