/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/ExceptionUtil.h"

export module Core:Exceptions;

import d1.types;
import d1.ostringstream;
import d1.stringConvert;
import d1.Exception;

import xml.Exceptions;


export namespace Core::Exceptions
{

class MandatoryAttributeMissing: public xml::ExceptionBase
{
public:
    MandatoryAttributeMissing(d1::uint32 n, const std::wstring& name):
        ExceptionBase{ n, L"mandatory attribute \"" + name + L"\" is missing" } {}
};

class AttributeHasWrongType: public xml::ExceptionBase
{
public:
    AttributeHasWrongType(d1::uint32 n, const std::wstring& name):
        ExceptionBase{ n, L"attribute \"" + name + L"\" has wrong type" } {}
};

class UnknownObjectID: public xml::ExceptionBase
{
public:
    UnknownObjectID(d1::uint32 n):
        ExceptionBase{ n, L"unknown reference" } {}
};

class TypeMismatchOfRestoredObject: public xml::ExceptionBase
{
public:
    TypeMismatchOfRestoredObject(d1::uint32 n):
        ExceptionBase{ n, L"type mismatch of restored object" } {}
};


class AttributeOutOfRange: public xml::ExceptionBase
{
public:
    AttributeOutOfRange(d1::uint32 n, const std::wstring& name):
        ExceptionBase{ n, L"attribute \"" + name + L"\" is out of range" } {}
};


class UnknownAttributeRestoreError: public xml::ExceptionBase
{
public:
    UnknownAttributeRestoreError(d1::uint32 n, const std::wstring& name):
        ExceptionBase{ n, L"attribute \"" + name + L"\" could be restored" } {}
};


class Base: public d1::Exception
{
protected:
    ~Base() = default;
};


D1_DEFINE_EXCEPTION(Core, Base, DuplicateObjectID)
D1_DEFINE_EXCEPTION(Core, Base, NullObjectID)
D1_DEFINE_EXCEPTION(Core, Base, InvalidObjectID)


D1_DEFINE_EXCEPTION(Core, Base, DuplicateNamedObject)

D1_DEFINE_EXCEPTION(Core, Base, DuplicateSingletonObject)




class Base2: public d1::Exception
{
public:
    using string = std::string;
    using wstring = std::wstring;

private:
    string msg_;

protected:
    d1::ostringstream out_;
    string w2s(const wstring& w) const { return d1::wstring2string(w); }

public:
    Base2():
        out_(msg_) {}
    const char* what() const { return msg_.c_str(); }

    Base2(const Base2& rhs):
        msg_(rhs.msg_), out_(msg_) {}
    Base2& operator=(const Base2& rhs)
    {
        msg_ = rhs.msg_;
        return *this;
    }
};


class OpenFileFailed: public Base2
{
public:
    OpenFileFailed(const wstring& filename)
    {
        out_ << "Could not open file " << w2s(filename);
    }
};

}
