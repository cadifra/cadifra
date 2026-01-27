/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil.Registry;


namespace
{
constexpr int InitialBufSize = 30;
}

namespace WinUtil::Registry
{

Key open(HKEY k, SubKeyName name, REGSAM sam)
{
    HKEY sk = 0;
    LONG r = ::RegOpenKeyEx(k, name.c_str(), 0, sam, &sk);

    if (r != ERROR_SUCCESS)
        throw Exception{ r };

    return Key{ sk };
}


Key create(HKEY k, SubKeyName name, REGSAM sam)
{
    HKEY sk = 0;
    LONG r = ::RegCreateKeyEx(
        k, name.c_str(), 0, (LPWSTR)L"", REG_OPTION_NON_VOLATILE, sam, 0, &sk, 0);

    if (r != ERROR_SUCCESS)
        throw Exception{ r };

    return Key{ sk };
}


bool query(HKEY k, ValueName name, std::wstring& val)
{
    auto buf = std::vector<BYTE>(InitialBufSize, 0);
    DWORD size = static_cast<DWORD>(buf.size());
    DWORD type = REG_NONE;

    LONG r = ::RegQueryValueEx(
        k, name.c_str(), 0, &type, buf.data(), &size);

    if (r == ERROR_MORE_DATA)
    {
        buf.resize(size);
        r = ::RegQueryValueEx(
            k, name.c_str(), 0, &type, buf.data(), &size);
    }

    if (r == ERROR_SUCCESS)
    {
        if (type != REG_SZ or size < sizeof(wchar_t))
            return false;

        val.assign(reinterpret_cast<wchar_t*>(buf.data()), size / sizeof(wchar_t) - 1);
        return true;
    }

    if (r == ERROR_FILE_NOT_FOUND)
        return false;

    throw Exception{ r };
    return false;
}


bool query(HKEY k, ValueName name, std::vector<std::wstring>& val)
{
    DWORD size = 0;
    DWORD type = REG_NONE;

    LONG r = ::RegQueryValueEx(k, name.c_str(), 0, &type, 0, &size);

    if (r == ERROR_FILE_NOT_FOUND)
        return false;

    if (type != REG_MULTI_SZ)
        return false;

    if (size < 2 * sizeof(wchar_t))
        return false;

    if (r != ERROR_SUCCESS)
    {
        throw Exception{ r };
        return false;
    }

    auto buf = std::vector<BYTE>(size);

    r = ::RegQueryValueEx(
        k, name.c_str(), 0, &type, buf.data(), &size);

    if (r != ERROR_SUCCESS)
        throw Exception{ r };

    std::div_t t = std::div(static_cast<int>(size), static_cast<int>(sizeof(wchar_t)));

    if (t.rem != 0)
        return false;

    val.clear();

    using It = const wchar_t*;
    auto i = reinterpret_cast<It>(buf.data());
    auto end = reinterpret_cast<It>(buf.data() + size);

    for (;;)
    {
        It t = i;

        for (; t != end and *t; ++t)
            ;

        if (i == t)
            return true;

        val.push_back(std::wstring(i, t));

        i = t;

        if (i == end)
            return true;

        ++i;
    }
    return true;
}


bool query(HKEY k, ValueName name, d1::bytestring& val)
{
    auto buf = d1::bytestring(InitialBufSize, 0);
    DWORD size = static_cast<DWORD>(buf.size());
    DWORD type = REG_NONE;

    LONG r = ::RegQueryValueEx(
        k, name.c_str(), 0, &type, reinterpret_cast<LPBYTE>(buf.data()), &size);

    if (r == ERROR_MORE_DATA)
    {
        buf = d1::bytestring(size, 0);
        r = ::RegQueryValueEx(
            k, name.c_str(), 0, &type, reinterpret_cast<LPBYTE>(buf.data()), &size);
    }

    if (r == ERROR_SUCCESS)
    {
        if (type == REG_BINARY and size > 0)
        {
            buf.resize(size);
            val.swap(buf);
            return true;
        }
        return false;
    }

    if (r == ERROR_FILE_NOT_FOUND)
        return false;

    throw Exception{ r };
    return false;
}


bool query(HKEY k, ValueName name, HGLOBAL& val, UINT uFlags)
{
    DWORD size = 0;
    DWORD type = REG_NONE;

    LONG r = ::RegQueryValueEx(k, name.c_str(), 0, &type, 0, &size);

    if (not(r == ERROR_SUCCESS and type == REG_BINARY and size > 0))
        return false;

    HGLOBAL g = ::GlobalAlloc(uFlags, size);
    if (not g)
        return false;

    LPVOID v = ::GlobalLock(g);

    if (v)
        r = ::RegQueryValueEx(
            k, name.c_str(), 0, &type, reinterpret_cast<LPBYTE>(v), &size);

    ::GlobalUnlock(g);

    if (r != ERROR_SUCCESS)
    {
        ::GlobalFree(v);
        return false;
    }

    val = v;
    return true;
}


bool query(HKEY k, ValueName name, LPBYTE val, DWORD size)
{
    DWORD s = size;
    DWORD type = REG_NONE;

    LONG r = ::RegQueryValueEx(k, name.c_str(), 0, &type, 0, &s);

    if (r == ERROR_FILE_NOT_FOUND)
        return false;

    if (type != REG_BINARY)
        return false;

    if (r == ERROR_SUCCESS)
    {
        if (s != size)
            return false;

        r = ::RegQueryValueEx(k, name.c_str(), 0, &type, val, &s);

        if (r == ERROR_SUCCESS)
            return true;
    }

    throw Exception{ r };
    return false;
}


bool query(HKEY k, ValueName name, d1::int32& val)
{
    d1::int32 buf = 0;
    DWORD size = sizeof(buf);
    DWORD type = REG_NONE;

    LONG r = ::RegQueryValueEx(
        k, name.c_str(), 0, &type, reinterpret_cast<LPBYTE>(&buf), &size);

    if (r == ERROR_MORE_DATA)
    {
        return false; // wrong type
    }

    if (r == ERROR_SUCCESS)
    {
        if (type == REG_DWORD and size == sizeof(buf))
        {
            val = buf;
            return true;
        }
        else
            return false;
    }

    if (r == ERROR_FILE_NOT_FOUND)
        return false;

    throw Exception{ r };
    return false;
}


bool query(HKEY k, ValueName name, bool& val)
{
    d1::int32 t = 0;
    if (not query(k, name, t))
        return false;
    else
    {
        val = t != 0;
        return true;
    }
}


void set(HKEY k, ValueName name, const std::wstring& val)
{
    LONG r = ::RegSetValueEx(k, name.c_str(), 0, REG_SZ,
        reinterpret_cast<CONST BYTE*>(val.c_str()), static_cast<DWORD>(val.size()) + 1);

    if (r != ERROR_SUCCESS)
        throw Exception{ r };
}



void set(HKEY k, ValueName name, const std::vector<std::wstring>& val)
{
    std::wstring s;

    for (const auto& ele : val)
    {
        s.append(ele);
        s.append(1, 0);
    }

    s.append(1, 0);

    LONG r = ::RegSetValueEx(k, name.c_str(), 0, REG_MULTI_SZ,
        reinterpret_cast<CONST BYTE*>(s.c_str()), static_cast<DWORD>(s.size()));

    if (r != ERROR_SUCCESS)
        throw Exception{ r };
}


void set(HKEY k, ValueName name, const d1::bytestring& val)
{
    LONG r = ::RegSetValueEx(k, name.c_str(), 0, REG_BINARY,
        reinterpret_cast<CONST BYTE*>(val.data()), static_cast<DWORD>(val.size()));

    if (r != ERROR_SUCCESS)
        throw Exception{ r };
}


void set(HKEY k, ValueName name, const HGLOBAL val)
{
    LONG r = ERROR_INVALID_PARAMETER;
    DWORD s = static_cast<DWORD>(::GlobalSize(val));
    LPVOID v = ::GlobalLock(val);

    if (v and s)
        r = ::RegSetValueEx(
            k, name.c_str(), 0, REG_BINARY, reinterpret_cast<CONST BYTE*>(v), s);

    ::GlobalUnlock(val);

    if (r != ERROR_SUCCESS)
        throw Exception{ r };
}


void set(HKEY k, ValueName name, CONST BYTE* val, DWORD size)
{
    LONG r = ::RegSetValueEx(k, name.c_str(), 0, REG_BINARY, val, size);

    if (r != ERROR_SUCCESS)
        throw Exception{ r };
}


void set(HKEY k, ValueName name, const d1::int32 val)
{
    LONG r = ::RegSetValueEx(k, name.c_str(), 0, REG_DWORD,
        reinterpret_cast<CONST BYTE*>(&val), sizeof(val));

    if (r != ERROR_SUCCESS)
        throw Exception{ r };
}


void set(HKEY k, ValueName name, const bool val)
{
    d1::int32 t = val ? 1 : 0;
    set(k, name, t);
}


Key::Letter::Letter(HKEY k):
    key_{ k }
{
}


Key::Letter::~Letter()
{
    ::RegCloseKey(key_);
}


Key::Key(HKEY k):
    letter_{ std::make_shared<Letter>(k) }
{
}


Exception::Exception(LONG err):
    error_(err),
    string_(255, 0)
{
    DWORD res = ::FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM,
        0,   // lpSource
        err, // dwMessageId
        0,   // dwLanguageId
        string_.data(),
        static_cast<DWORD>(string_.size()),
        0 // Arguments
    );

    string_.resize(res);

    string_ = "Registry access error:\n" + string_;
}


const char* Exception::what() const
{
    return string_.c_str();
}

}
