/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module WinUtil;

import :CException;

import std;


namespace WinUtil::CException
{

Common::Common()
{
}


Common::Common(const EXCEPTION_RECORD& er):
    itsFlags{ er.ExceptionFlags },
    itsAddress{ er.ExceptionAddress }
{
}


const char* Common::what() const
{
    if (itsBuffer.empty())
    {
        auto ost = std::ostringstream{};
        ost << "CException::" << GetName() << " thrown at address " << itsAddress
            << std::endl;

        itsBuffer = ost.str();
    }

    return itsBuffer.c_str();
}


Access_violation::Access_violation(const EXCEPTION_RECORD& er):
    Common{ er },
    itWasAWrite{ er.ExceptionInformation[0] == 1 },
    itsAccessAddress{ reinterpret_cast<void*>(er.ExceptionInformation[1]) }
{
}


const char* Access_violation::what() const
{
    if (itsBuffer.empty())
    {
        Common::what();
        auto ost = std::stringstream{};
        ost << "Tried to " << (itWasAWrite ? "write to" : "read from")
            << " address " << itsAccessAddress << std::endl;

        itsBuffer += ost.str();
    }

    return itsBuffer.c_str();
}

}
