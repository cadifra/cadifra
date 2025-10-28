/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

module WinUtil.ResourceLoader;


namespace WinUtil
{

namespace
{

class Impl: public ResourceLoader
{
public:
    Impl();

    void Init(HINSTANCE) final;
    std::wstring GetString(UINT id) const final;
    std::wstring GetFormatString(
        UINT id, const StringListType& list) const final;

    HICON GetIcon(WORD id) const final;
    HACCEL GetAccelerators(WORD id) const final;
    RootMenuHandle GetMenu(WORD id) const final;
    HCURSOR GetCursor(WORD id) const final;
    GdiObjectOwner<HBITMAP> GetBitmap(WORD id) const final;
    HINSTANCE GetInstanceHandle() const final;

private:
    HINSTANCE itsInstance;
};


Impl::Impl():
    itsInstance{ ::GetModuleHandle(0) }
{
}


void Impl::Init(HINSTANCE inst)
{
    itsInstance = inst;
}


HINSTANCE Impl::GetInstanceHandle() const
{
    D1_ASSERT(itsInstance);
    return itsInstance;
}


std::wstring Impl::GetString(UINT id) const
{
    using std::wstring;

    const int InitialLength = 256;
    // arbitrary number that should be enough for most resource strings.

    const int MaxLength = 100 * 1024;
    // maximum string length

    D1_ASSERT(itsInstance);
    int len = InitialLength;
    wstring s;

    for (;;)
    {
        s.resize(len);

        int res = ::LoadString(
            itsInstance,
            id,
            const_cast<wstring::value_type*>(s.c_str()),
            static_cast<int>(s.size())
        );

        if (res == 0)
        {
            // string not found
            return L"Resource not found";
        }
        else if ((res == len) || (res == len - 1))
        {
            // the buffer was probably to small

            if (len == MaxLength)
            {
                return L"Resource string too long";
            }

            // enlarge the buffer and try again
            len *= 2;
            len = std::min(MaxLength, len);
        }
        else
        {
            s.resize(res);
            return s;
        }
    }
}


struct LocalAllocBuffer
{
    LPVOID itsAddress{};
    LocalAllocBuffer() {}
    ~LocalAllocBuffer() { ::LocalFree(itsAddress); }
};


std::wstring Impl::GetFormatString(
    UINT id, const StringListType& list) const
{
    // ## WARNING: If the format string specified by id contains placeholders
    //             %1..%n, but there are less than n actual entries in list,
    //             the behavior of GetFormatString() is not defined.

    using std::wstring;

    const int num_miss = 10;

    auto arg = std::vector<const wchar_t*>{};
    arg.reserve(list.size() + num_miss + 1);

    for (const auto& s : list)
        arg.push_back(s.c_str());

    for (int i = num_miss; i; --i)
        arg.push_back(L"[error: missing parameter in format]");

    arg.push_back(0);

    auto format = wstring(this->GetString(id));

    auto buf = LocalAllocBuffer{}; // will hold address of buffer allocated by following
                                   //::FormatMessage call

    // Bad: FormatMessage has no way to check how many arguments are actually passed
    // in parameter va_list.
    DWORD res = ::FormatMessage(
        // DWORD dwFlags,      // source and processing options
        FORMAT_MESSAGE_ALLOCATE_BUFFER |   // FormatMessage allocates a buffer for output
            FORMAT_MESSAGE_FROM_STRING |   // parameter lpSource is pointer to message definition
            FORMAT_MESSAGE_ARGUMENT_ARRAY, // parameter va_list is pointer to array of pointers to
                                           // null terminated argument strings

        // LPCVOID lpSource,   // message source
        format.c_str(),

        // DWORD dwMessageId,  // message identifier
        0,

        // DWORD dwLanguageId, // language identifier
        0,

        // LPTSTR lpBuffer,    // message buffer
        reinterpret_cast<LPTSTR>(&buf.itsAddress),

        // DWORD nSize,        // minimum number of TCHARS to allocate for output buffer
        0,

        // va_list *Arguments  // array of message inserts
        const_cast<char**>(reinterpret_cast<const char**>(&arg.front())));

    auto s = wstring(static_cast<const wchar_t*>(buf.itsAddress));

    return s;
}


HICON Impl::GetIcon(WORD id) const
{
    D1_ASSERT(itsInstance);
    return static_cast<HICON>(::LoadImage(
        itsInstance,
        MAKEINTRESOURCE(id),
        IMAGE_ICON,
        0, // cxDesired
        0, // cyDesired
        LR_SHARED));
}


HACCEL Impl::GetAccelerators(WORD id) const
{
    D1_ASSERT(itsInstance);
    return ::LoadAccelerators(itsInstance, MAKEINTRESOURCE(id));
}


RootMenuHandle Impl::GetMenu(WORD id) const
{
    D1_ASSERT(itsInstance);
    return RootMenuHandle{ ::LoadMenu(itsInstance, MAKEINTRESOURCE(id)) };
}


HCURSOR Impl::GetCursor(WORD id) const
{
    D1_ASSERT(itsInstance);
    return static_cast<HCURSOR>(::LoadImage(
        itsInstance,
        MAKEINTRESOURCE(id),
        IMAGE_CURSOR,
        0, // cxDesired
        0, // cyDesired
        LR_SHARED));
}


auto Impl::GetBitmap(WORD id) const
    -> GdiObjectOwner<HBITMAP>
{
    D1_ASSERT(itsInstance);
    return GdiObjectOwner<HBITMAP>{
        static_cast<HBITMAP>(
            ::LoadImage(
                itsInstance,
                MAKEINTRESOURCE(id),
                IMAGE_BITMAP,
                0, // cxDesired
                0, // cyDesired
                LR_DEFAULTCOLOR))
    };
}

}


auto ResourceLoader::Instance() -> ResourceLoader&
{
    static Impl impl;
    return impl;
}

}
