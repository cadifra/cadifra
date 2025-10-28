/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.Registry;

import d1.types;
import d1.Exception;

import std;


export namespace WinUtil::Registry
{

class Key;
using SubKeyName = const std::wstring&;
using ValueName = const std::wstring&;

// -----------------------------------------------------------------------------
// The functions Open, Create, Query and Set may throw "Exception"

Key Open(HKEY, SubKeyName, REGSAM = KEY_QUERY_VALUE);
Key Create(HKEY, SubKeyName, REGSAM = KEY_WRITE);


// If the value does not exist or has not the expected type, Query returns
// false and leaves 'val' unchanged.
bool Query(HKEY, ValueName, std::wstring& val);
bool Query(HKEY, ValueName, std::vector<std::wstring>& val);
bool Query(HKEY, ValueName, d1::int32& val);
bool Query(HKEY, ValueName, bool& val);
bool Query(HKEY, ValueName, d1::bytestring& val);
bool Query(HKEY, ValueName, HGLOBAL& val, UINT uFlags); // uFlags see GlobalAlloc
bool Query(HKEY, ValueName, LPBYTE val, DWORD size);

template <class T>
bool QueryBinary(HKEY k, ValueName name, T& val)
{
    return Query(k, name, reinterpret_cast<LPBYTE>(&val), sizeof(T));
}


void Set(HKEY, ValueName, const std::wstring& val);
void Set(HKEY, ValueName, const std::vector<std::wstring>& val);
void Set(HKEY, ValueName, const d1::int32 val);
void Set(HKEY, ValueName, const bool val);
void Set(HKEY, ValueName, const d1::bytestring& val);
void Set(HKEY, ValueName, const HGLOBAL val);
void Set(HKEY, ValueName, const BYTE* val, DWORD size);

template <class T>
void SetBinary(HKEY k, ValueName name, const T& val)
{
    Set(k, name, reinterpret_cast<const BYTE*>(&val), sizeof(T));
}


// -----------------------------------------------------------------------------
// A Key object owns a HKEY handle. The HKEY is closed if the last copy of the
// Key object is destructed.

class Key
{
public:
    explicit Key(HKEY); // takes ownership of the key-handle

    operator HKEY() const { return itsLetter->itsKey; }

    // uses compiler generated copy-ctor, dtor and assignment-operator

private:
    struct Letter // Key uses envelope-letter pattern
    {
        HKEY itsKey;
        Letter(HKEY k);
        ~Letter();
    };
    std::shared_ptr<Letter> itsLetter;
};


// -----------------------------------------------------------------------------

class Exception: public d1::Exception
{
    LONG itsError;
    std::string itsString;

public:
    Exception(LONG err);
    const char* what() const;
    LONG GetError() const { return itsError; }
};

}
