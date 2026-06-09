/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>
#include <objidl.h>

export module App.DataProvider;

import App.IFormatProvider;

import d1.AutoComPtr;


namespace App
{

export bool detectInProcess(IUnknown* obj);
// Returns true if "obj" lives in the same process as the caller
// and implements "IDetectInProcess".


export class DataProvider // has value semantic
{
public:
    DataProvider();
    ~DataProvider();
    DataProvider(const DataProvider&);
    DataProvider& operator=(const DataProvider&);

    void GetDataObject(IDataObject**);

    void addFormat(IFormatProvider&); // does an addref

    void SendOnStop();
    void SendOnDataChange();

private:
    class Impl;

    d1::AutoComPtr<Impl> impl_;
};

}
