/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module WinUtil.CException;

import std;


namespace WinUtil::CException
{

Common::Common()
{
}


Common::Common(const EXCEPTION_RECORD& er):
    flags_{ er.ExceptionFlags },
    address_{ er.ExceptionAddress }
{
}


const char* Common::what() const
{
    if (buffer_.empty())
    {
        auto ost = std::ostringstream{};
        ost << "CException::" << getName() << " thrown at address " << address_
            << std::endl;

        buffer_ = ost.str();
    }

    return buffer_.c_str();
}


Access_violation::Access_violation(const EXCEPTION_RECORD& er):
    Common{ er },
    itWasAWrite{ er.ExceptionInformation[0] == 1 },
    accessAddress_{ reinterpret_cast<void*>(er.ExceptionInformation[1]) }
{
}


const char* Access_violation::what() const
{
    if (buffer_.empty())
    {
        Common::what();
        auto ost = std::stringstream{};
        ost << "Tried to " << (itWasAWrite ? "write to" : "read from")
            << " address " << accessAddress_ << std::endl;

        buffer_ += ost.str();
    }

    return buffer_.c_str();
}

}
