/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module xml.Exceptions;

import d1.stringConvert;


namespace xml
{

using std::string;
using std::wstring;

namespace
{

inline std::string line_msg(d1::uint32 n, const string& m)
{
    auto os = std::ostringstream{};
    os << "line " << n << ": " << m;
    return os.str();
}

}


ExceptionBase::ExceptionBase(d1::uint32 n, const string& m):
    msg_{ line_msg(n, m) }
{
}


ExceptionBase::ExceptionBase(d1::uint32 n, const wstring& m):
    ExceptionBase{ n, d1::wstring2string(m) }
{
}

}
