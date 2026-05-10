/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module App.ObjectDescriptor;

import App.Com;

import d1.Point;
import d1.Size;

import std;


namespace App
{

export class ObjectDescriptor
{
public:
    CLSID clsid = CLSID_NULL;
    std::wstring fullUserTypeName;
    std::wstring sourceOfCopy;
    d1::Size size = {};
    d1::Point dragPoint = {};
    DWORD status = 0;
    DWORD drawAspect = 0;

    ObjectDescriptor(
        const CLSID& clsid,
        const std::wstring& sourceOfCopy,
        const d1::Size& size = d1::Size{},
        const d1::Point& dragPoint = d1::Point{},
        DWORD drawAspect = DVASPECT_CONTENT);
    // fullUserTypeName and drawAspect are set according to the
    // values found in the registry for 'clsid'.

    ObjectDescriptor(
        const CLSID& clsid,
        const std::wstring& fullUserTypeName,
        const std::wstring& sourceOfCopy,
        const d1::Size& size,
        const d1::Point& dragPoint,
        DWORD status,
        DWORD drawAspect);

    ObjectDescriptor();
    ObjectDescriptor(const ObjectDescriptor&) = default;
    ObjectDescriptor& operator=(const ObjectDescriptor&) = default;
};


export namespace ObjectDescriptorFormatHandler
{
void addFormatProvider(
    DataProvider& dp, const ObjectDescriptor& od);
// Adds a new FormatProvider that contains the ObjectDescriptor od
// into the DataProvider object dp.

bool QueryExtract(IDataHolder& dh);
// Returns true if dh contains an ObjectDescriptor.

bool extract(IDataHolder& dh, ObjectDescriptor& od);
// If dh contains an ObjectDescriptor, the function extracts it and
// returns true. Otherwise it returns false and doesn't change od.
}

}
