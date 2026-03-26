/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module xml.Exceptions;

import d1.Exception;
import d1.types;

import std;


namespace xml
{

using std::string;
using std::wstring;

export class ExceptionBase: public d1::Exception
{
    string msg_;

protected:
    ExceptionBase(d1::uint32 line_number, const string& message);
    ExceptionBase(d1::uint32 line_number, const wstring& message);

public:
    const char* what() const { return msg_.c_str(); }
};


export namespace Exceptions
{

#define D1_XML_EXCEPTION(name, text)               \
    class name: public ExceptionBase               \
    {                                              \
    public:                                        \
        name(d1::uint32 n): ExceptionBase(n, text) \
        {                                          \
        }                                          \
    };

// clang-format off

D1_XML_EXCEPTION(CDATAsecNotClosed,        "CDATA section not closed")
D1_XML_EXCEPTION(ColonNotAllowedInPItarget,"colon not allowed in processing instruction target")
D1_XML_EXCEPTION(CommentNotClosed,         "comment not closed")

D1_XML_EXCEPTION(DoubleHyphenInComment,    "the string \"--\" is not allowed inside comments")
D1_XML_EXCEPTION(DuplicateSlashInTag,      "duplicate slash in tag")
D1_XML_EXCEPTION(DuplicateColonInTag,      "duplicate colon char in tag")
D1_XML_EXCEPTION(DuplicateNamespace,       "duplicate namespace declaration")

D1_XML_EXCEPTION(EndOfCDATAsecNotAllowed,  "the literal string \"]]>\" is not allowed in this context")
D1_XML_EXCEPTION(ExpectedEqualSign,        "expected equal sign")
D1_XML_EXCEPTION(ExpectedGT,               "expected closing > sign")
D1_XML_EXCEPTION(ExpectedXMLprolog,        "expected xml prolog (\"<?xml\")")

D1_XML_EXCEPTION(IllegalChar,              "found illegal character")
D1_XML_EXCEPTION(IllegalCharInName,        "illegal character in name")

D1_XML_EXCEPTION(MatchingEndQuoteNotFound, "matching closing \" sign not found")
D1_XML_EXCEPTION(MatchingGTnotFound,       "matching > not found")
D1_XML_EXCEPTION(MatchingPiGTnotFound,     "matching ?> not found")

D1_XML_EXCEPTION(MissingBeginQuote,        "missing beginning \" sign")
D1_XML_EXCEPTION(MissingEndEntity,         "missing closing ; sign of &entity;")
D1_XML_EXCEPTION(MissingName,              "missing name")
D1_XML_EXCEPTION(MissingNamespacePrefix,   "missing namespace prefix")
D1_XML_EXCEPTION(MissingTagName,           "missing tag name")
D1_XML_EXCEPTION(MissingXMLprolog,         "missing xml prolog")

D1_XML_EXCEPTION(NoElementFound,           "no element found")

D1_XML_EXCEPTION(PrefixXMLnotAllowed,      "namespace prefix must not start with reserved string \"xml\"")

D1_XML_EXCEPTION(UnexpectedEndOfLine,      "unexpected end of line")
D1_XML_EXCEPTION(UnexpectedEndOfFile,      "unexpected end of file")
D1_XML_EXCEPTION(UnexpectedEndOfElement,   "unexpected end of element")

D1_XML_EXCEPTION(WhiteSpaceNotAllowedHere, "whitespace is not allowed here")

// clang-format on



class AttributeMissingNamespacePrefix: public ExceptionBase
{
public:
    AttributeMissingNamespacePrefix(d1::uint32 n, const wstring& name):
        ExceptionBase{ n, L"attribute \"" + name + L"\": missing namespace prefix" }
    {
    }
};

class AttributeDuplicateColon: public ExceptionBase
{
public:
    AttributeDuplicateColon(d1::uint32 n, const wstring& name):
        ExceptionBase{ n, L"attribute \"" + name + L"\": duplicate colon" }
    {
    }
};

class AttributeNotClosed: public ExceptionBase
{
public:
    AttributeNotClosed(d1::uint32 n, const wstring& name):
        ExceptionBase{ n, L"closing quote sign of attribute \"" + name + L"\" not found" }
    {
    }
};

class LessThanCharInAttribute: public ExceptionBase
{
public:
    LessThanCharInAttribute(d1::uint32 n, const wstring& name):
        ExceptionBase{ n, L"less than char inside attribute \"" + name + L"\" is not allowed" }
    {
    }
};

class NotSupported: public ExceptionBase
{
public:
    NotSupported(d1::uint32 n, const wstring& found):
        ExceptionBase{ n, L"\"" + found + L"\" is not supported" }
    {
    }
};

class EncodingNotSupported: public ExceptionBase
{
public:
    EncodingNotSupported(d1::uint32 n, const wstring& encoding):
        ExceptionBase{ n, L"encoding=\"" + encoding + L"\" is not supported" }
    {
    }
};

class StandaloneNotSupported: public ExceptionBase
{
public:
    StandaloneNotSupported(d1::uint32 n, const wstring& standalone):
        ExceptionBase{ n, L"standalone=\"" + standalone + L"\" is not supported" }
    {
    }
};

class WrongElement: public ExceptionBase
{
public:
    WrongElement(d1::uint32 n, const wstring& expected, const wstring& found):
        ExceptionBase{ n,
            L"Wrong Element. Expected \"" + expected + L"\", found \"" + found + L"\"" }
    {
    }
};

class WrongAttribute: public ExceptionBase
{
public:
    WrongAttribute(d1::uint32 n, const wstring& expected, const wstring& found):
        ExceptionBase{ n, L"Wrong attribute. Expected " + expected + L", found " + found }
    {
    }
};

class IllegalAttribute: public ExceptionBase
{
public:
    IllegalAttribute(d1::uint32 n, const wstring& name):
        ExceptionBase{ n, L"illegal attribute \"" + name + L"\"" }
    {
    }
};

class DuplicateAttribute: public ExceptionBase
{
public:
    DuplicateAttribute(d1::uint32 n, const wstring& name):
        ExceptionBase{ n, L"duplicate attribute \"" + name + L"\"" }
    {
    }
};

class VersionNotSupported: public ExceptionBase
{
public:
    VersionNotSupported(d1::uint32 n, const wstring& found):
        ExceptionBase{ n, L"version " + found + L" is not supported" }
    {
    }
};

class UnexpectedTag: public ExceptionBase
{
public:
    UnexpectedTag(d1::uint32 n, const wstring& tagname):
        ExceptionBase{ n, L"unexpected tag \"" + tagname + L"\"" }
    {
    }
};

class UnknownElement: public ExceptionBase
{
public:
    UnknownElement(d1::uint32 n, const wstring& tagname):
        ExceptionBase{ n, L"Unknown element \"" + tagname + L"\"" }
    {
    }
};

class EndTagDoesNotMatchStartTag: public ExceptionBase
{
public:
    EndTagDoesNotMatchStartTag(d1::uint32 n, const wstring& qualified_tagname):
        ExceptionBase{ n, L"end tag \"</" + qualified_tagname + L">\" does not match start tag" }
    {
    }
};

class EndTagWithoutStartTag: public ExceptionBase
{
public:
    EndTagWithoutStartTag(d1::uint32 n, const wstring& qualified_tagname):
        ExceptionBase{ n, L"end tag \"</" + qualified_tagname + L">\" does not have a start tag" }
    {
    }
};

class AttributeNotFound: public ExceptionBase
{
public:
    AttributeNotFound(d1::uint32 n, const wstring& name):
        ExceptionBase{ n, L"attribute \"" + name + L"\" not found" }
    {
    }
};

class NamespaceDeclarationMissing: public ExceptionBase
{
public:
    NamespaceDeclarationMissing(d1::uint32 n, const wstring& name):
        ExceptionBase{ n, L"declaration for namespace \"" + name + L"\" is missing" }
    {
    }
};

class NamespaceForElementNotSupported: public ExceptionBase
{
public:
    NamespaceForElementNotSupported(d1::uint32 n, const wstring& ns, const wstring& ele):
        ExceptionBase{ n,
            L"namespace \"" + ns + L"\" is not supported for element \"" + ele + L"\"" }
    {
    }
};

class NamespaceForPrefixMissing: public ExceptionBase
{
public:
    NamespaceForPrefixMissing(d1::uint32 n, const wstring& name):
        ExceptionBase{ n, L"no namespace declaration found for prefix \"" + name + L"\"" }
    {
    }
};

class ConversionFailed: public ExceptionBase
{
public:
    ConversionFailed(d1::uint32 n, const wstring& name, const wstring& rep):
        ExceptionBase{ n, L"cannot convert " + name + L" from \"" + rep + L"\"" }
    {
    }
};

class EmptyAttribute: public ExceptionBase
{
public:
    EmptyAttribute(d1::uint32 n, const wstring& name):
        ExceptionBase{ n, L"xml attribute \"" + name + L"\" is empty" }
    {
    }
};

class InvalidEntity: public ExceptionBase
{
public:
    InvalidEntity(d1::uint32 n, const wstring& name):
        ExceptionBase{ n, L"invalid or unknown entity \"&" + name + L";\"" }
    {
    }
};

class InvalidCharacterReference: public ExceptionBase
{
public:
    InvalidCharacterReference(d1::uint32 n, const wstring& name):
        ExceptionBase{ n, L"invalid character reference \"&" + name + L";\"" }
    {
    }
};


}

}
