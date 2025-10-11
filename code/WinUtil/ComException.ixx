/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil:ComException;

import std;

import d1.Exception;


namespace WinUtil
{

export class ComException: public d1::Exception
{
public:
    //-- std::exception

    const char* what() const override;
    // The returned pointer is only valid until the next
    // call of ComException::what

    //--

    static void Check(HRESULT r)
    {
        if (FAILED(r))
            throw ComException{ r };
    }

    ComException(HRESULT r):
        itsHRESULT{ r }
    {
    }

    HRESULT GetHRESULT() const { return itsHRESULT; }

    std::string GetHRESULT_string(
        bool hex = true, bool with_prefix = true, bool in_parens = true) const;

    // uses compiler generated copy ctor and assignment operator

private:
    HRESULT itsHRESULT;

    static std::string lastWhat;
    static std::string lastMessage;
};

}
