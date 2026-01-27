/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>
#include <comdef.h>

module WinUtil.WinOstream;

import d1.stringConvert;

import std;


std::ostream& operator<<(std::ostream& os, const GUID& id)
{
    LPOLESTR s;
    HRESULT res = ProgIDFromCLSID(id, &s);
    if (SUCCEEDED(res))
    {
        os << '{' << std::wstring(s) << '}';
        CoTaskMemFree(s);
    }
    else
    {
        res = StringFromCLSID(id, &s);
        if (SUCCEEDED(res))
        {
            auto idstring = std::wstring(s);
            CoTaskMemFree(s);

            auto subkey = std::wstring(L"Interface\\" + idstring);

            HKEY k;

            if (ERROR_SUCCESS == RegOpenKeyEx(
                                     HKEY_CLASSES_ROOT,
                                     subkey.c_str(),
                                     0,
                                     KEY_QUERY_VALUE,
                                     &k))
            {
                DWORD type = 0;
                auto buff = std::vector<BYTE>(100);
                DWORD buffsize = static_cast<DWORD>(buff.size());

                if (ERROR_SUCCESS == RegQueryValueEx(
                                         k,
                                         0,
                                         0,
                                         &type,
                                         buff.data(),
                                         &buffsize))
                {
                    auto s = std::wstring(reinterpret_cast<const wchar_t*>(buff.data()),
                        buffsize / sizeof(wchar_t));
                    os << s;
                }
                else
                    os << idstring;

                RegCloseKey(k);
            }
            else
                os << idstring;
        }
    }
    return os;
}


std::ostream& operator<<(std::ostream& os, const std::wstring ws)
{
    os << d1::wstring2string(ws);
    return os;
}


std::ostream& operator<<(std::ostream& os, const FORMATETC& fetc)
{
    os << "{" << PrintCLIPFORMAT(fetc.cfFormat) << ",";

    if (fetc.ptd)
        os << *fetc.ptd;
    else
        os << "{NULL}";

    os << "," << fetc.dwAspect
       << "," << fetc.lindex
       << "," << PrintTYMED(fetc.tymed) << "}";

    return os;
}


std::ostream& operator<<(std::ostream& os, const DVTARGETDEVICE& dv)
{
    os << "{";

    const BYTE* begin = reinterpret_cast<const BYTE*>(&dv);

    if (dv.tdDriverNameOffset)
    {
        os << std::wstring(reinterpret_cast<LPCOLESTR>(
            begin + dv.tdDriverNameOffset));
    }
    os << ",";
    if (dv.tdDeviceNameOffset)
    {
        os << std::wstring(reinterpret_cast<LPCOLESTR>(
            begin + dv.tdDeviceNameOffset));
    }
    os << ",";
    if (dv.tdPortNameOffset)
    {
        os << std::wstring(reinterpret_cast<LPCOLESTR>(
            begin + dv.tdPortNameOffset));
    }

    os << "}";
    return os;
}


std::ostream& operator<<(std::ostream& os, const STGMEDIUM& stgmed)
{
    os << "{" << PrintTYMED(stgmed.tymed) << ",";

    switch (stgmed.tymed)
    {
    case TYMED_GDI:
        os << stgmed.hBitmap;
        break;
    case TYMED_HGLOBAL:
        os << stgmed.hGlobal;
        break;
    case TYMED_FILE:
        os << std::wstring(stgmed.lpszFileName);
        break;
    case TYMED_ISTREAM:
        os << stgmed.pstm;
        break;
    case TYMED_ISTORAGE:
        os << stgmed.pstg;
        break;
    case TYMED_MFPICT:
        os << stgmed.hMetaFilePict;
        break;
    case TYMED_ENHMF:
        os << stgmed.hEnhMetaFile;
        break;
    case TYMED_NULL:
        os << 0;
        break;
    default:
        os << "invalid";
    }

    os << "," << stgmed.pUnkForRelease << "}";

    return os;
}


std::ostream& operator<<(std::ostream& os, const DATADIR& dataDir)
{
    switch (dataDir)
    {
    case DATADIR_GET:
        os << "DATADIR_GET";
        break;
    case DATADIR_SET:
        os << "DATADIR_SET";
        break;
    default:
        os << "invalid";
    }
    return os;
}


std::ostream& operator<<(std::ostream& os, const OLEVERB& oleverb)
{
    os << "{" << oleverb.lVerb;
    os << ", \"" << std::wstring(oleverb.lpszVerbName) << "\", ";
    os << oleverb.fuFlags << ", " << oleverb.grfAttribs << "}";
    return os;
}


std::ostream& operator<<(std::ostream& os, const POINT& p)
{
    os << "{" << p.x << "," << p.y << "}";
    return os;
}


std::ostream& operator<<(std::ostream& os, const RECT& r)
{
    os << "{(" << r.left << "," << r.top << "),(";
    os << r.right << "," << r.bottom << ")}";
    return os;
}


std::ostream& operator<<(std::ostream& os, const RECT* pr)
{
    if (pr != 0)
        os << *pr;
    else
        os << static_cast<const void*>(pr);

    return os;
}


std::ostream& operator<<(std::ostream& os, const SIZE& s)
{
    os << "{" << s.cx << "," << s.cy << "}";
    return os;
}


std::ostream& operator<<(std::ostream& os, LPCOLESTR& s)
{
    os << d1::wstring2string(s);
    return os;
}


std::ostream& operator<<(std::ostream& os, HRGN rgn)
{
    RECT r;
    int res = GetRgnBox(rgn, &r);

    switch (res)
    {
    case NULLREGION:
        os << "NULLREGION = " << r;
        break;
    case SIMPLEREGION:
        os << "SIMPLEREGION = " << r;
        break;
    case COMPLEXREGION:
        os << "COMPLEXREGION RgnBox = " << r;
        break;
    default:
        os << "invalid region";
    }
    return os;
}


void PrintCLIPFORMAT::print(std::ostream& os) const
{
    switch (m_)
    {
    case CF_TEXT:
        os << "CF_TEXT";
        break;
    case CF_BITMAP:
        os << "CF_BITMAP";
        break;
    case CF_METAFILEPICT:
        os << "CF_METAFILEPICT";
        break;
    case CF_SYLK:
        os << "CF_SYLK";
        break;
    case CF_DIF:
        os << "CF_DIF";
        break;
    case CF_TIFF:
        os << "CF_TIFF";
        break;
    case CF_OEMTEXT:
        os << "CF_OEMTEXT";
        break;
    case CF_DIB:
        os << "CF_DIB";
        break;
    case CF_PALETTE:
        os << "CF_PALETTE";
        break;
    case CF_PENDATA:
        os << "CF_PENDATA";
        break;
    case CF_RIFF:
        os << "CF_RIFF";
        break;
    case CF_WAVE:
        os << "CF_WAVE";
        break;
    case CF_UNICODETEXT:
        os << "CF_UNICODETEXT";
        break;
    case CF_ENHMETAFILE:
        os << "CF_ENHMETAFILE";
        break;
    case CF_HDROP:
        os << "CF_HDROP";
        break;
    case CF_LOCALE:
        os << "CF_LOCALE";
        break;
    case CF_MAX:
        os << "CF_MAX";
        break;
    case CF_OWNERDISPLAY:
        os << "CF_OWNERDISPLAY";
        break;
    case CF_DSPTEXT:
        os << "CF_DSPTEXT";
        break;
    case CF_DSPBITMAP:
        os << "CF_DSPBITMAP";
        break;
    case CF_DSPMETAFILEPICT:
        os << "CF_DSPMETAFILEPICT";
        break;
    case CF_DSPENHMETAFILE:
        os << "CF_DSPENHMETAFILE";
        break;
    case CF_PRIVATEFIRST:
        os << "CF_PRIVATEFIRST";
        break;
    case CF_PRIVATELAST:
        os << "CF_PRIVATELAST";
        break;
    case CF_GDIOBJFIRST:
        os << "CF_GDIOBJFIRST";
        break;
    case CF_GDIOBJLAST:
        os << "CF_GDIOBJLAST";
        break;
    default:
        const int maxNameLen = 30;
        TCHAR formatName[maxNameLen];

        if (GetClipboardFormatName(m_, formatName, maxNameLen))
            os << d1::wstring2string(formatName);
        else
            os << m_;
    }
}


namespace
{
bool testClear(DWORD* candidate, const DWORD raster)
{
    if (*candidate & raster)
    {
        *candidate ^= raster;
        return true;
    }
    return false;
}
}

void PrintTYMED::print(std::ostream& os) const
{
    DWORD tymed = m_;

    if (tymed == TYMED_NULL)
        os << "TYMED_NULL ";
    if (testClear(&tymed, TYMED_HGLOBAL))
        os << "TYMED_HGLOBAL ";
    if (testClear(&tymed, TYMED_FILE))
        os << "TYMED_FILE ";
    if (testClear(&tymed, TYMED_ISTREAM))
        os << "TYMED_ISTREAM ";
    if (testClear(&tymed, TYMED_ISTORAGE))
        os << "TYMED_ISTORAGE ";
    if (testClear(&tymed, TYMED_GDI))
        os << "TYMED_GDI ";
    if (testClear(&tymed, TYMED_MFPICT))
        os << "TYMED_MFPICT ";
    if (testClear(&tymed, TYMED_ENHMF))
        os << "TYMED_ENHMF ";
    if (testClear(&tymed, TYMED_NULL))
        os << "TYMED_NULL ";
    if (tymed)
        os << tymed; // rest
}


void PrintADVF::print(std::ostream& os) const
{
    DWORD advf = m_;
    if (advf == 0)
        os << "no ADVF set";
    if (testClear(&advf, ADVF_NODATA))
        os << "ADVF_NODATA ";
    if (testClear(&advf, ADVF_PRIMEFIRST))
        os << "ADVF_PRIMEFIRST ";
    if (testClear(&advf, ADVF_ONLYONCE))
        os << "ADVF_ONLYONCE ";
    if (testClear(&advf, ADVF_DATAONSTOP))
        os << "ADVF_DATAONSTOP ";
    if (testClear(&advf, ADVFCACHE_NOHANDLER))
        os << "ADVFCACHE_NOHANDLER ";
    if (testClear(&advf, ADVFCACHE_FORCEBUILTIN))
        os << "ADVFCACHE_FORCEBUILTIN ";
    if (testClear(&advf, ADVFCACHE_ONSAVE))
        os << "ADVFCACHE_ONSAVE ";
}


void PrintHRESULT::print(std::ostream& os) const
{
    if (m_ == S_OK)
        os << "S_OK";
    else if (m_ == S_FALSE)
        os << "S_FALSE";
    else
    {
        auto e = _com_error{ m_ };
        os << std::hex << m_ << " = " << d1::wstring2string(e.ErrorMessage());
    }
}
