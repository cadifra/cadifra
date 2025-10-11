/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.UTF8cvt;

import std;


export namespace d1
{

std::wstring ConvertFromUTF8(const std::string&);
std::string ConvertToUTF8(const std::wstring&);


using UTF8cvtBase = std::codecvt<wchar_t, char, std::mbstate_t>;

class UTF8cvt: public UTF8cvtBase
{
public:
    explicit UTF8cvt(size_t ref = 0);

protected:
    virtual result do_out(
        state_type& state,
        const intern_type* from,
        const intern_type* from_end,
        const intern_type*& from_next,
        extern_type* to,
        extern_type* to_limit,
        extern_type*& to_next) const;

    virtual result do_in(
        state_type& state,
        const extern_type* from,
        const extern_type* from_end,
        const extern_type*& from_next,
        intern_type* to,
        intern_type* to_limit,
        intern_type*& to_next) const;

    virtual result do_unshift(
        state_type& state,
        extern_type* to,
        extern_type* to_limit,
        extern_type*& to_next) const;

    virtual int do_encoding() const throw();

    virtual bool do_always_noconv() const throw();

    virtual int do_length(
        state_type& state,
        const extern_type* from,
        const extern_type* end,
        size_t len) const;

    virtual int do_max_length() const throw();
};

}
