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

    void init(HINSTANCE) final;
    std::wstring getString(UINT id) const final;
    std::wstring getFormatString(
        UINT id, const StringListType& list) const final;

    HICON getIcon(WORD id) const final;
    HACCEL getAccelerators(WORD id) const final;
    RootMenuHandle getMenu(WORD id) const final;
    HCURSOR getCursor(WORD id) const final;
    GdiObjectOwner<HBITMAP> getBitmap(WORD id) const final;
    HINSTANCE getInstanceHandle() const final;

private:
    HINSTANCE instance_;
};


Impl::Impl():
    instance_{ ::GetModuleHandle(0) }
{
}


void Impl::init(HINSTANCE inst)
{
    instance_ = inst;
}


HINSTANCE Impl::getInstanceHandle() const
{
    D1_ASSERT(instance_);
    return instance_;
}


std::wstring Impl::getString(UINT id) const
{
    using std::wstring;

    const int InitialLength = 256;
    // arbitrary number that should be enough for most resource strings.

    const int MaxLength = 100 * 1024;
    // maximum string length

    D1_ASSERT(instance_);
    int len = InitialLength;
    wstring s;

    for (;;)
    {
        s.resize(len);

        int res = ::LoadString(
            instance_,
            id,
            const_cast<wstring::value_type*>(s.c_str()),
            static_cast<int>(s.size())
        );

        if (res == 0)
        {
            // string not found
            return L"Resource not found";
        }
        else if ((res == len) or (res == len - 1))
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
    LPVOID address_{};
    LocalAllocBuffer() {}
    ~LocalAllocBuffer() { ::LocalFree(address_); }
};


std::wstring Impl::getFormatString(
    UINT id, const StringListType& list) const
{
    // ## WARNING: If the format string specified by id contains placeholders
    //             %1..%n, but there are less than n actual entries in list,
    //             the behavior of getFormatString() is not defined.

    using std::wstring;

    const int num_miss = 10;

    auto arg = std::vector<const wchar_t*>{};
    arg.reserve(list.size() + num_miss + 1);

    for (const auto& s : list)
        arg.push_back(s.c_str());

    for (int i = num_miss; i; --i)
        arg.push_back(L"[error: missing parameter in format]");

    arg.push_back(0);

    auto format = wstring(this->getString(id));

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
        reinterpret_cast<LPTSTR>(&buf.address_),

        // DWORD nSize,        // minimum number of TCHARS to allocate for output buffer
        0,

        // va_list *Arguments  // array of message inserts
        const_cast<char**>(reinterpret_cast<const char**>(&arg.front())));

    auto s = wstring(static_cast<const wchar_t*>(buf.address_));

    return s;
}


HICON Impl::getIcon(WORD id) const
{
    D1_ASSERT(instance_);
    return static_cast<HICON>(::LoadImage(
        instance_,
        MAKEINTRESOURCE(id),
        IMAGE_ICON,
        0, // cxDesired
        0, // cyDesired
        LR_SHARED));
}


HACCEL Impl::getAccelerators(WORD id) const
{
    D1_ASSERT(instance_);
    return ::LoadAccelerators(instance_, MAKEINTRESOURCE(id));
}


RootMenuHandle Impl::getMenu(WORD id) const
{
    D1_ASSERT(instance_);
    return RootMenuHandle{ ::LoadMenu(instance_, MAKEINTRESOURCE(id)) };
}


HCURSOR Impl::getCursor(WORD id) const
{
    D1_ASSERT(instance_);
    return static_cast<HCURSOR>(::LoadImage(
        instance_,
        MAKEINTRESOURCE(id),
        IMAGE_CURSOR,
        0, // cxDesired
        0, // cyDesired
        LR_SHARED));
}


auto Impl::getBitmap(WORD id) const
    -> GdiObjectOwner<HBITMAP>
{
    D1_ASSERT(instance_);
    return GdiObjectOwner<HBITMAP>{
        static_cast<HBITMAP>(
            ::LoadImage(
                instance_,
                MAKEINTRESOURCE(id),
                IMAGE_BITMAP,
                0, // cxDesired
                0, // cyDesired
                LR_DEFAULTCOLOR))
    };
}

}


auto ResourceLoader::instance() -> ResourceLoader&
{
    static Impl impl;
    return impl;
}

}
