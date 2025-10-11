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
    static void OutImpl(wchar_t, char*&) {}
    static bool RecursiveInCheck(const char*) { return true; }
    static wchar_t InImpl(const char*&) { return 0; }
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

    static void OutImpl(wchar_t c, char*& to_next)
    {
        *to_next = Marker | c >> PayloadShift & PayloadMask;
        NextByte::OutImpl(c, ++to_next);
    }

    static bool Out(
        const wchar_t* from_end, const wchar_t*& from_next,
        char* to_limit, char*& to_next,
        result& res)
    {
        D1_ASSERT(from_end - from_next >= 1);

        if (*from_next >= 1 << TotalPayloadBitCount)
            return false;

        if (to_limit - to_next < TotalByteCount)
        {
            res = std::codecvt_base::partial;
            return true;
        }

        OutImpl(*from_next, to_next);
        from_next++;
        return true;
    }


    static bool InCheck(const char* from_next)
    {
        return (*from_next & MarkerMask) == Marker;
    }

    static bool RecursiveInCheck(const char* from_next)
    {
        return InCheck(from_next) && NextByte::RecursiveInCheck(from_next + 1);
    }

    static wchar_t InImpl(const char*& from_next)
    {
        const char c = *from_next++;
        return ((c & PayloadMask) << PayloadShift) | NextByte::InImpl(from_next);
    }

    static bool In(
        wchar_t* to_limit, wchar_t*& to_next,
        const char* from_end, const char*& from_next,
        result& res)
    {
        D1_ASSERT(from_end - from_next >= 1);

        if (!InCheck(from_next))
            return false;

        if (from_end - from_next < TotalByteCount)
        {
            res = std::codecvt_base::partial;
            return true;
        }

        if (!NextByte::RecursiveInCheck(from_next + 1))
        {
            res = std::codecvt_base::error;
            return true;
        }

        if (to_next >= to_limit)
        {
            res = std::codecvt_base::partial;
            return true;
        }

        *to_next = InImpl(from_next);
        to_next++;
        return true;
    }


    static bool Advance(const char* from_end, const char*& from_next)
    {
        if (from_end - from_next < TotalByteCount)
            return false;
        if (!RecursiveInCheck(from_next))
            return false;
        from_next += TotalByteCount;
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
    const UTF8cvt::extern_type* from_end,
    const UTF8cvt::extern_type*& from_next,
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
    const UTF8cvt::extern_type* i = from_next;

    for (; *bom && i < from_end; ++i, ++bom)
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
    from_next = i;
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
    const intern_type* from_end,
    const intern_type*& from_next,
    extern_type* to,
    extern_type* to_limit,
    extern_type*& to_next) const
{
    to_next = to;
    from_next = from;
    result res = ok;

    while (from_next < from_end && res == ok)
    {
        if (!OneUTF8byte::Out(from_end, from_next, to_limit, to_next, res))
            if (!TwoUTF8bytes::Out(from_end, from_next, to_limit, to_next, res))
                if (!ThreeUTF8bytes::Out(from_end, from_next, to_limit, to_next, res))
                    if (!FourUTF8bytes::Out(from_end, from_next, to_limit, to_next, res))
                        res = error;
    }
    return res;
}


UTF8cvt::result UTF8cvt::do_in(
    state_type& state,
    const extern_type* from,
    const extern_type* from_end,
    const extern_type*& from_next,
    intern_type* to,
    intern_type* to_limit,
    intern_type*& to_next) const
{
    to_next = to;
    from_next = from;
    result res = ok;

    if (eatBOM(state, from_end, from_next, res))
        return res;

    while (from_next < from_end && res == ok)
    {
        if (!OneUTF8byte::In(to_limit, to_next, from_end, from_next, res))
            if (!TwoUTF8bytes::In(to_limit, to_next, from_end, from_next, res))
                if (!ThreeUTF8bytes::In(to_limit, to_next, from_end, from_next, res))
                    if (!FourUTF8bytes::In(to_limit, to_next, from_end, from_next, res))
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
    const extern_type* from_next = from;

    // According to the ISO C++ Standard, Second edition 2003-10-15, the state
    // parameter should not be const. But the std library from dinkum asks for a
    // do_length function with a "const state_type&" (this seems to be a bug).
    state_type state_copy = state;
    result dummy = ok;
    if (eatBOM(state_copy, end, from_next, dummy))
        return static_cast<int>(from_next - from);

    for (; len > 0; len--)
    {
        if (!OneUTF8byte::Advance(end, from_next))
            if (!TwoUTF8bytes::Advance(end, from_next))
                if (!ThreeUTF8bytes::Advance(end, from_next))
                    if (!FourUTF8bytes::Advance(end, from_next))
                        break;
    }
    return static_cast<int>(from_next - from);
}


int UTF8cvt::do_max_length() const throw()
{
    return MaxByteCount;
}


std::wstring ConvertFromUTF8(const std::string& in)
{
    std::wstring res;
    res.reserve(in.size());

    std::codecvt_base::result errCode = std::codecvt_base::ok;
    const char* from_next = in.data();
    const char* from_end = from_next + in.size();

    while (from_next < from_end)
    {
        wchar_t c;
        wchar_t* to_next = &c;
        wchar_t* to_limit = &c + 1;

        if (!OneUTF8byte::In(to_limit, to_next, from_end, from_next, errCode))
            if (!TwoUTF8bytes::In(to_limit, to_next, from_end, from_next, errCode))
                if (!ThreeUTF8bytes::In(to_limit, to_next, from_end, from_next, errCode))
                    if (!FourUTF8bytes::In(to_limit, to_next, from_end, from_next, errCode))
                        errCode = std::codecvt_base::error;

        if (errCode != std::codecvt_base::ok)
            break;

        res.push_back(c);
    }

    return res;
}


std::string ConvertToUTF8(const std::wstring& in)
{
    std::string res;
    res.reserve(in.size());

    std::codecvt_base::result errCode = std::codecvt_base::ok;
    const wchar_t* from_next = in.data();
    const wchar_t* from_end = from_next + in.size();

    while (from_next < from_end)
    {
        char c[MaxByteCount];
        char* to_next = c;
        char* to_limit = c + MaxByteCount;

        if (!OneUTF8byte::Out(from_end, from_next, to_limit, to_next, errCode))
            if (!TwoUTF8bytes::Out(from_end, from_next, to_limit, to_next, errCode))
                if (!ThreeUTF8bytes::Out(from_end, from_next, to_limit, to_next, errCode))
                    if (!FourUTF8bytes::Out(from_end, from_next, to_limit, to_next, errCode))
                        errCode = std::codecvt_base::error;

        if (errCode != std::codecvt_base::ok)
            break;

        res.insert(end(res), c, to_next);
    }

    return res;
}


}
