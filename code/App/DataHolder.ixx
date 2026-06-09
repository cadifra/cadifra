/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module App.DataHolder;

import App.IDataHolder;

import d1.AutoComPtr;

import std;


namespace App
{

// DataHolder is a wrapper that makes an IDataHolder from
// an IDataObject (COM).
// DataHolder has value semantics.

export class DataHolder: public IDataHolder
{
public:
    DataHolder() {}
    DataHolder(::IDataObject* o): dataObject_{ o } {}

    DataHolder(const DataHolder&);
    DataHolder& operator=(const DataHolder&);

    DataHolder(DataHolder&&);
    DataHolder& operator=(DataHolder&&);

    bool extract(IFormatExtractor&) override;
    bool QueryExtract(IFormatTester&) override;

private:
    d1::AutoComPtr<::IDataObject> dataObject_;
};


DataHolder::DataHolder(const DataHolder& rhs):
    dataObject_{ rhs.dataObject_ }
{
}


DataHolder::DataHolder(DataHolder&& rhs):
    dataObject_{ std::move(rhs.dataObject_) }
{
}


DataHolder& DataHolder::operator=(const DataHolder& rhs)
{
    dataObject_ = rhs.dataObject_;
    return *this;
}


DataHolder& DataHolder::operator=(DataHolder&& rhs)
{
    dataObject_ = std::move(rhs.dataObject_);
    return *this;
}


bool DataHolder::extract(IFormatExtractor& e)
{
    if (dataObject_)
        return e.extract(dataObject_);
    else
        return false;
}


bool DataHolder::QueryExtract(IFormatTester& t)
{
    if (dataObject_)
        return t.QueryExtract(dataObject_);
    else
        return false;
}

}
