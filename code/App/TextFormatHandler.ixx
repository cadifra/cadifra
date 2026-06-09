/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

export module App.TextFormatHandler;

import App.IDataHolder;
import App.DataProvider;

import std;


export namespace App::TextFormatHandler
{

void addFormatProvider(
    DataProvider& dp, const std::wstring& t, LCID lcid = LOCALE_USER_DEFAULT);
// Adds a new FormatProviders that contains the text t into
// the DataProvider object dp.

bool QueryExtract(IDataHolder& dh);
// Returns true if dh contains a text format.

void extract(IDataHolder& dh, std::wostream& o);
// If dh contains a text, the function writes the text into o

std::wstring extract(IDataHolder& dh);
// Returns the text that is in the clipboard dh or an empty string
// if there is no text in dh.

}

module : private;

import App.IFormatProvider;

import d1.AutoComPtr;

import WinUtil.Clipboard;
import WinUtil.OwnerSTGMEDIUM;
import WinUtil.SetCharsetInfo;
import WinUtil.StreamConnector;


namespace App
{

namespace
{

enum
{
    IndexUnicode,
    IndexLocale,
    IndexText,
    FormatCount
};

const FORMATETC Formats[] = {
    { CF_UNICODETEXT,
        0,
        DVASPECT_CONTENT,
        -1,
        TYMED_HGLOBAL | TYMED_ISTREAM },
    { CF_LOCALE,
        0,
        DVASPECT_CONTENT,
        -1,
        TYMED_HGLOBAL | TYMED_ISTREAM },
    { CF_TEXT,
        0,
        DVASPECT_CONTENT,
        -1,
        TYMED_HGLOBAL | TYMED_ISTREAM }
};


class TextFormatProvider: public App::IFormatProvider
{
    const std::wstring string_;
    const LCID lcid_;
    std::string convertedString_;

    int GetFormatCount() const final { return FormatCount; }

    void GetFormat(int index, tagFORMATETC&) const final;

    HRESULT GetData(
        int index,
        tagFORMATETC* pFormatetc,
        tagSTGMEDIUM* pmedium) final;

    HRESULT GetDataHere(
        int index,
        tagFORMATETC* pFormatetc,
        tagSTGMEDIUM* pmedium) final;

    const std::string& convert();

public:
    TextFormatProvider(const std::wstring& s, LCID lcid);
};


auto TextFormatProvider::convert() -> const std::string&
{
    if (not convertedString_.empty() or string_.empty())
        return convertedString_;

    CHARSETINFO csinfo;
    UINT cp = WinUtil::fromLCID(csinfo, lcid_) ? csinfo.ciACP : CP_ACP;

    int size = WideCharToMultiByte(cp, WC_COMPOSITECHECK, string_.c_str(),
        static_cast<int>(string_.size()),
        0, 0, 0, 0
    );

    auto buf = std::vector<CHAR>(size);

    size = WideCharToMultiByte(cp, WC_COMPOSITECHECK, string_.c_str(),
        static_cast<int>(string_.size()),
        buf.data(),
        static_cast<int>(buf.size()),
        0, 0
    );

    if (size > 0)
        convertedString_.assign(buf.data(), size);

    return convertedString_;
}


void TextFormatProvider::GetFormat(int formatIndex, tagFORMATETC& fetc) const
{
    D1_ASSERT(0 <= formatIndex);
    D1_ASSERT(formatIndex < FormatCount);
    fetc = Formats[formatIndex];
}


HRESULT TextFormatProvider::GetData(
    int formatIndex,
    tagFORMATETC* pFormatetc,
    tagSTGMEDIUM* pmedium)
{
    using WinUtil::Clipboard::copy;

    switch (formatIndex)
    {
    case IndexUnicode:
        return copy(string_, pFormatetc, pmedium);

    case IndexLocale:
        return copy(&lcid_, sizeof(lcid_), pFormatetc, pmedium);

    case IndexText:
        return copy(convert(), pFormatetc, pmedium);

    default:
        return E_NOTIMPL;
    }
}


HRESULT TextFormatProvider::GetDataHere(
    int formatIndex,
    tagFORMATETC* pFormatetc,
    tagSTGMEDIUM* pmedium)
{
    using WinUtil::Clipboard::copy;

    if (not pFormatetc or not pmedium)
        return E_INVALIDARG;

    if (pFormatetc->tymed != TYMED_ISTREAM)
        return E_NOTIMPL;

    if ((pmedium->tymed != TYMED_ISTREAM) or (pmedium->pstm == 0))
        return E_INVALIDARG;

    switch (formatIndex)
    {
    case IndexUnicode:
        return copy(string_, pmedium->pstm);

    case IndexLocale:
        return pmedium->pstm->Write(&lcid_, sizeof(lcid_), 0);

    case IndexText:
        return copy(convert(), pmedium->pstm);

    default:
        return E_NOTIMPL;
    }
}


TextFormatProvider::TextFormatProvider(const std::wstring& s, LCID lcid):
    string_{ s },
    lcid_{ lcid }
{
}


class TextFormatTester: public App::IFormatTester
{
public:
    bool QueryExtract(IDataObject*) final;
};


bool TextFormatTester::QueryExtract(IDataObject* data)
{
    auto fetc = FORMATETC{
        .cfFormat = CF_UNICODETEXT,
        .ptd = 0,
        .dwAspect = DVASPECT_CONTENT,
        .lindex = -1,
        .tymed = TYMED_ISTREAM | TYMED_HGLOBAL
    };

    return S_OK == data->QueryGetData(&fetc);
}


class TextFormatExtractor: public App::IFormatExtractor
{
public:
    TextFormatExtractor(std::wostream&);
    bool extract(IDataObject*) final;

private:
    std::wostream& OStream_;
};


TextFormatExtractor::TextFormatExtractor(std::wostream& s):
    OStream_{ s }
{
}


bool TextFormatExtractor::extract(IDataObject* data)
{
    FORMATETC fetc = Formats[IndexUnicode];
    fetc.tymed = TYMED_ISTREAM;

    auto sc = d1::AutoComPtr<IStream>{ new WinUtil::StreamConnector(OStream_) };
    auto stg = WinUtil::OwnerSTGMEDIUM{ sc };
    auto p = OStream_.tellp();

    if (SUCCEEDED(data->GetDataHere(&fetc, &stg)))
        return true;

    fetc.tymed = TYMED_ISTREAM | TYMED_HGLOBAL;
    stg.Free();
    OStream_.seekp(p);

    if (FAILED(data->GetData(&fetc, &stg)))
        return false;

    return SUCCEEDED(WinUtil::Clipboard::copy(stg, sc)) ? true : false;
}

}

namespace TextFormatHandler
{

void addFormatProvider(
    App::DataProvider& dp, const std::wstring& t, LCID lcid)
{
    dp.addFormat(*new TextFormatProvider(t, lcid));
}


bool QueryExtract(IDataHolder& dh)
{
    auto te = TextFormatTester{};
    return dh.QueryExtract(te);
}


void extract(IDataHolder& dh, std::wostream& o)
{
    auto te = TextFormatExtractor(o);
    dh.extract(te);
}


std::wstring extract(IDataHolder& dh)
{
    auto o = std::wostringstream{};
    auto te = TextFormatExtractor(o);
    dh.extract(te);
    return o.str();
}

}

}
