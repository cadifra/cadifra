/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1assert.h"

module d1.UTF8cvt;


#pragma warning(disable : 4333) // '>>' : right shift by too large amount, data loss


namespace d1
{

namespace
{


struct UTF8byteEndByteTag
{
    enum
    {
        TotalPayloadBitCount = 0,
        TotalByteCount = 0
    };
    static void outImpl(wchar_t, char*&) {}
    static bool recursiveInCheck(const char*) { return true; }
    static wchar_t inImpl(const char*&) { return 0; }
};


template <int PayloadBitCount, class NextByte = UTF8byteEndByteTag>
class UTF8byte
{
public:
    using result = std::codecvt_base::result;

    enum
    {
        ByteMask = (unsigned char)(~0),
        PayloadMask = (1 << PayloadBitCount) - 1,
        MarkerMask = ~PayloadMask & ByteMask,
        Marker = (-1 << (PayloadBitCount + 1)) & ByteMask,
        TotalPayloadBitCount = PayloadBitCount + NextByte::TotalPayloadBitCount,
        PayloadShift = NextByte::TotalPayloadBitCount,
        TotalByteCount = 1 + NextByte::TotalByteCount
    };

    static void outImpl(wchar_t c, char*& to_next)
    {
        *to_next = Marker | c >> PayloadShift & PayloadMask;
        NextByte::outImpl(c, ++to_next);
    }

    static bool out(
        const wchar_t* froend_, const wchar_t*& fronext_,
        char* to_limit, char*& to_next,
        result& res)
    {
        D1_ASSERT(froend_ - fronext_ >= 1);

        if (*fronext_ >= 1 << TotalPayloadBitCount)
            return false;

        if (to_limit - to_next < TotalByteCount)
        {
            res = std::codecvt_base::partial;
            return true;
        }

        outImpl(*fronext_, to_next);
        fronext_++;
        return true;
    }


    static bool inCheck(const char* fronext_)
    {
        return (*fronext_ & MarkerMask) == Marker;
    }

    static bool recursiveInCheck(const char* fronext_)
    {
        return inCheck(fronext_) and NextByte::recursiveInCheck(fronext_ + 1);
    }

    static wchar_t inImpl(const char*& fronext_)
    {
        const char c = *fronext_++;
        return ((c & PayloadMask) << PayloadShift) | NextByte::inImpl(fronext_);
    }

    static bool in(
        wchar_t* to_limit, wchar_t*& to_next,
        const char* froend_, const char*& fronext_,
        result& res)
    {
        D1_ASSERT(froend_ - fronext_ >= 1);

        if (not inCheck(fronext_))
            return false;

        if (froend_ - fronext_ < TotalByteCount)
        {
            res = std::codecvt_base::partial;
            return true;
        }

        if (not NextByte::recursiveInCheck(fronext_ + 1))
        {
            res = std::codecvt_base::error;
            return true;
        }

        if (to_next >= to_limit)
        {
            res = std::codecvt_base::partial;
            return true;
        }

        *to_next = inImpl(fronext_);
        to_next++;
        return true;
    }


    static bool advance(const char* froend_, const char*& fronext_)
    {
        if (froend_ - fronext_ < TotalByteCount)
            return false;
        if (not recursiveInCheck(fronext_))
            return false;
        fronext_ += TotalByteCount;
        return true;
    }
};

using OneUTF8byte = UTF8byte<7>;
using TwoUTF8bytes = UTF8byte<5, UTF8byte<6>>;
using ThreeUTF8bytes = UTF8byte<4, UTF8byte<6, UTF8byte<6>>>;
using FourUTF8bytes = UTF8byte<3, UTF8byte<6, UTF8byte<6, UTF8byte<6>>>>;

constexpr int MaxByteCount = 4;



bool eatBOM(
    UTF8cvt::state_type& state,
    const UTF8cvt::extern_type* froend_,
    const UTF8cvt::extern_type*& fronext_,
    std::codecvt_base::result& res)
{
    enum
    {
        StreamStart = 0,
        StreamContent = 1
    };

    if (state._State != StreamStart)
        return false;

    const UTF8cvt::extern_type* bom = "\xEF\xBB\xBF";
    const UTF8cvt::extern_type* i = fronext_;

    for (; *bom and i < froend_; ++i, ++bom)
    {
        if (*bom != *i)
        {
            state._State = StreamContent;
            return false;
        }
    }

    res = std::codecvt_base::partial;
    if (*bom)
        return true;

    state._State = StreamContent;
    fronext_ = i;
    return false;
}


}



UTF8cvt::UTF8cvt(size_t ref):
    UTF8cvtBase(ref)
{
}


UTF8cvt::result UTF8cvt::do_out(
    state_type& state,
    const intern_type* from,
    const intern_type* froend_,
    const intern_type*& fronext_,
    extern_type* to,
    extern_type* to_limit,
    extern_type*& to_next) const
{
    to_next = to;
    fronext_ = from;
    result res = ok;

    while (fronext_ < froend_ and res == ok)
    {
        if (not OneUTF8byte::out(froend_, fronext_, to_limit, to_next, res))
            if (not TwoUTF8bytes::out(froend_, fronext_, to_limit, to_next, res))
                if (not ThreeUTF8bytes::out(froend_, fronext_, to_limit, to_next, res))
                    if (not FourUTF8bytes::out(froend_, fronext_, to_limit, to_next, res))
                        res = error;
    }
    return res;
}


UTF8cvt::result UTF8cvt::do_in(
    state_type& state,
    const extern_type* from,
    const extern_type* froend_,
    const extern_type*& fronext_,
    intern_type* to,
    intern_type* to_limit,
    intern_type*& to_next) const
{
    to_next = to;
    fronext_ = from;
    result res = ok;

    if (eatBOM(state, froend_, fronext_, res))
        return res;

    while (fronext_ < froend_ and res == ok)
    {
        if (not OneUTF8byte::in(to_limit, to_next, froend_, fronext_, res))
            if (not TwoUTF8bytes::in(to_limit, to_next, froend_, fronext_, res))
                if (not ThreeUTF8bytes::in(to_limit, to_next, froend_, fronext_, res))
                    if (not FourUTF8bytes::in(to_limit, to_next, froend_, fronext_, res))
                        res = error;
    }
    return res;
}


UTF8cvt::result UTF8cvt::do_unshift(
    state_type& state,
    extern_type* to,
    extern_type* to_limit,
    extern_type*& to_next) const
{
    to_next = to;
    return noconv; // no termination needed
}


int UTF8cvt::do_encoding() const throw()
{
    return 0; // 0 == varying number of bytes to represent individual characters
}


bool UTF8cvt::do_always_noconv() const throw()
{
    return false;
}


int UTF8cvt::do_length(
    state_type& state,
    const extern_type* from,
    const extern_type* end,
    size_t len) const
{
    const extern_type* fronext_ = from;

    // According to the ISO C++ Standard, Second edition 2003-10-15, the state
    // parameter should not be const. But the std library from dinkum asks for a
    // do_length function with a "const state_type&" (this seems to be a bug).
    state_type state_copy = state;
    result dummy = ok;
    if (eatBOM(state_copy, end, fronext_, dummy))
        return static_cast<int>(fronext_ - from);

    for (; len > 0; len--)
    {
        if (not OneUTF8byte::advance(end, fronext_))
            if (not TwoUTF8bytes::advance(end, fronext_))
                if (not ThreeUTF8bytes::advance(end, fronext_))
                    if (not FourUTF8bytes::advance(end, fronext_))
                        break;
    }
    return static_cast<int>(fronext_ - from);
}


int UTF8cvt::do_max_length() const throw()
{
    return MaxByteCount;
}


std::wstring convertFromUTF8(const std::string& in)
{
    std::wstring res;
    res.reserve(in.size());

    std::codecvt_base::result errCode = std::codecvt_base::ok;
    const char* fronext_ = in.data();
    const char* froend_ = fronext_ + in.size();

    while (fronext_ < froend_)
    {
        wchar_t c;
        wchar_t* to_next = &c;
        wchar_t* to_limit = &c + 1;

        if (not OneUTF8byte::in(to_limit, to_next, froend_, fronext_, errCode))
            if (not TwoUTF8bytes::in(to_limit, to_next, froend_, fronext_, errCode))
                if (not ThreeUTF8bytes::in(to_limit, to_next, froend_, fronext_, errCode))
                    if (not FourUTF8bytes::in(to_limit, to_next, froend_, fronext_, errCode))
                        errCode = std::codecvt_base::error;

        if (errCode != std::codecvt_base::ok)
            break;

        res.push_back(c);
    }

    return res;
}


std::string convertToUTF8(const std::wstring& in)
{
    std::string res;
    res.reserve(in.size());

    std::codecvt_base::result errCode = std::codecvt_base::ok;
    const wchar_t* fronext_ = in.data();
    const wchar_t* froend_ = fronext_ + in.size();

    while (fronext_ < froend_)
    {
        char c[MaxByteCount];
        char* to_next = c;
        char* to_limit = c + MaxByteCount;

        if (not OneUTF8byte::out(froend_, fronext_, to_limit, to_next, errCode))
            if (not TwoUTF8bytes::out(froend_, fronext_, to_limit, to_next, errCode))
                if (not ThreeUTF8bytes::out(froend_, fronext_, to_limit, to_next, errCode))
                    if (not FourUTF8bytes::out(froend_, fronext_, to_limit, to_next, errCode))
                        errCode = std::codecvt_base::error;

        if (errCode != std::codecvt_base::ok)
            break;

        res.insert(end(res), c, to_next);
    }

    return res;
}


}
