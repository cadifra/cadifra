/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.Clipboard;

import std;

typedef tagSTGMEDIUM STGMEDIUM;


namespace WinUtil
{

export namespace ClipboardHelpers
{

HRESULT copy(const void* from, size_t from_size,
    const tagFORMATETC* pFormatetc, tagSTGMEDIUM* to);

HRESULT copy(IStream* from, ::IStream* to);

HRESULT copy(HGLOBAL from, ::IStream* to);

HRESULT copy(const STGMEDIUM& from, ::IStream* to);


template <typename T>
HRESULT copy(const std::basic_string<T>& from,
    const tagFORMATETC* pFormatetc, tagSTGMEDIUM* to)
{
    return copy(
        from.c_str(),
        (from.size() + 1) * sizeof(std::basic_string<T>::value_type),
        pFormatetc,
        to);
}


template <typename T>
HRESULT copy(const std::basic_string<T>& from, ::IStream* to)
{
    return to->Write(
        from.c_str(),
        static_cast<ULONG>(
            (from.size() + 1) * sizeof(std::basic_string<T>::value_type)),
        0);
}

}

export namespace CommonClipboardFormats
{

UINT EmbedSource();
UINT LinkSource();
UINT ObjectDescriptor();
UINT EmbeddedObject();
UINT FileNameW();
UINT FileName();
UINT RichTextFormat();
UINT PNG();
UINT GIF();

}


/*
The class PrivateClipFormat allows the registration of an application local
clipboard format.

With this class, you do not need to maintain a global header file that contains
all private clipboard formats which creates unwanted dependencies between
unrelated packages.

PrivateClipFormat uses clipboard format numbers in the range
CF_PRIVATEFIRST...CF_PRIVATELAST.

Note the difference to the operating system function RegisterClipboardFormat,
which creates clipboard format numbers that are unique among all applications.
*/
export class PrivateClipFormat
{
public:
    static auto Instance() -> PrivateClipFormat&;

    virtual CLIPFORMAT Register() = 0;
    // returns a clipboard format number that is unique
    // within the application

protected:
    ~PrivateClipFormat() = default;
};

}
