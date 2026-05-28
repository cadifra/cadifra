/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

export module WinUtil.ClipboardFormats;


export namespace WinUtil::ClipboardFormats
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


module : private;


namespace WinUtil::ClipboardFormats
{

UINT EmbedSource()
{
    static UINT cf = ::RegisterClipboardFormat(
        TEXT("Embed Source"));

    D1_ASSERT(cf);
    return cf;
}


UINT LinkSource()
{
    static UINT cf = ::RegisterClipboardFormat(
        TEXT("Link Source"));

    D1_ASSERT(cf);
    return cf;
}


UINT ObjectDescriptor()
{
    static UINT cf = ::RegisterClipboardFormat(
        TEXT("Object Descriptor"));

    D1_ASSERT(cf);
    return cf;
}


UINT EmbeddedObject()
{
    static UINT cf = ::RegisterClipboardFormat(
        TEXT("Embedded Object"));

    D1_ASSERT(cf);
    return cf;
}


UINT FileNameW()
{
    static UINT cf = ::RegisterClipboardFormat(
        TEXT("FileNameW"));

    D1_ASSERT(cf);
    return cf;
}


UINT FileName()
{
    static UINT cf = ::RegisterClipboardFormat(
        TEXT("FileName"));

    D1_ASSERT(cf);
    return cf;
}


UINT RichTEXTFormat()
{
    static UINT cf = ::RegisterClipboardFormat(
        TEXT("Rich TEXT Format"));

    D1_ASSERT(cf);
    return cf;
}


UINT PNG()
{
    static UINT cf = ::RegisterClipboardFormat(TEXT("PNG"));
    D1_ASSERT(cf);
    return cf;
}


UINT GIF()
{
    static UINT cf = ::RegisterClipboardFormat(TEXT("GIF"));
    D1_ASSERT(cf);
    return cf;
}

}
