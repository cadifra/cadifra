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

Key open(HKEY, SubKeyName, REGSAM = KEY_QUERY_VALUE);
Key create(HKEY, SubKeyName, REGSAM = KEY_WRITE);


// If the value does not exist or has not the expected type, Query returns
// false and leaves 'val' unchanged.
bool query(HKEY, ValueName, std::wstring& val);
bool query(HKEY, ValueName, std::vector<std::wstring>& val);
bool query(HKEY, ValueName, d1::int32& val);
bool query(HKEY, ValueName, bool& val);
bool query(HKEY, ValueName, d1::bytestring& val);
bool query(HKEY, ValueName, HGLOBAL& val, UINT uFlags); // uFlags see GlobalAlloc
bool query(HKEY, ValueName, LPBYTE val, DWORD size);

template <class T>
bool queryBinary(HKEY k, ValueName name, T& val)
{
    return query(k, name, reinterpret_cast<LPBYTE>(&val), sizeof(T));
}


void set(HKEY, ValueName, const std::wstring& val);
void set(HKEY, ValueName, const std::vector<std::wstring>& val);
void set(HKEY, ValueName, const d1::int32 val);
void set(HKEY, ValueName, const bool val);
void set(HKEY, ValueName, const d1::bytestring& val);
void set(HKEY, ValueName, const HGLOBAL val);
void set(HKEY, ValueName, const BYTE* val, DWORD size);

template <class T>
void setBinary(HKEY k, ValueName name, const T& val)
{
    set(k, name, reinterpret_cast<const BYTE*>(&val), sizeof(T));
}


// -----------------------------------------------------------------------------
// A Key object owns a HKEY handle. The HKEY is closed if the last copy of the
// Key object is destructed.

class Key
{
public:
    explicit Key(HKEY); // takes ownership of the key-handle

    operator HKEY() const { return letter_->key_; }

    // uses compiler generated copy-ctor, dtor and assignment-operator

private:
    struct Letter // Key uses envelope-letter pattern
    {
        HKEY key_;
        Letter(HKEY k);
        ~Letter();
    };
    std::shared_ptr<Letter> letter_;
};


// -----------------------------------------------------------------------------

class Exception: public d1::Exception
{
    LONG error_;
    std::string string_;

public:
    Exception(LONG err);
    const char* what() const;
    LONG getError() const { return error_; }
};

}
