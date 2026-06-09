/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>
#include <objidl.h>

export module App.IDataHolder;

namespace App
{

export class IFormatExtractor
{
public:
    virtual bool extract(::IDataObject*) = 0;

protected:
    ~IFormatExtractor() = default;
};


export class IFormatTester
{
public:
    virtual bool QueryExtract(::IDataObject*) = 0;

protected:
    ~IFormatTester() = default;
};


export class IDataHolder
{
public:
    virtual ~IDataHolder() = default;

    virtual bool extract(IFormatExtractor&) = 0;
    virtual bool QueryExtract(IFormatTester&) = 0;
};

}
