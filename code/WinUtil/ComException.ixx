/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil.ComException;

import std;

import d1.Exception;
import d1.wintypes;


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

    static void check(d1::HRESULT r);

    ComException(d1::HRESULT r):
        HRESULT_{ r }
    {
    }

    d1::HRESULT getHRESULT() const { return HRESULT_; }

    std::string getHRESULT_string(
        bool hex = true, bool with_prefix = true, bool in_parens = true) const;

    // uses compiler generated copy ctor and assignment operator

private:
    d1::HRESULT HRESULT_;

    static std::string lastWhat;
    static std::string lastMessage;
};

}
