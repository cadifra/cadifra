/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

module WinUtil.ExtractDEVNAMES;

import WinUtil.Global;


namespace WinUtil
{

HGLOBAL createDEVNAMES(
    const std::wstring& device,
    const std::wstring& driver,
    const std::wstring& output)
{
    HGLOBAL g = GlobalAlloc(GHND,
        sizeof(DEVNAMES) + (device.size() + 1 + driver.size() +
                               1 + output.size() + 1) *
                               sizeof(TCHAR));

    auto d = GlobalLocker<DEVNAMES>{ g };

    d->wDriverOffset = sizeof(DEVNAMES) / sizeof(TCHAR);
    d->wDeviceOffset = d->wDriverOffset + static_cast<WORD>(driver.size()) + 1;
    d->wOutputOffset = d->wDeviceOffset + static_cast<WORD>(device.size()) + 1;

    TCHAR* i = reinterpret_cast<TCHAR*>(d.getPtr());

    CopyMemory(i + d->wDriverOffset, driver.c_str(), (driver.size() + 1) * sizeof(TCHAR));
    CopyMemory(i + d->wDeviceOffset, device.c_str(), (device.size() + 1) * sizeof(TCHAR));
    CopyMemory(i + d->wOutputOffset, output.c_str(), (output.size() + 1) * sizeof(TCHAR));

    return g;
}


ExtractDEVNAMES::ExtractDEVNAMES(HGLOBAL g)
{
    auto d = GlobalLocker<DEVNAMES>{ g };

    const TCHAR* i = reinterpret_cast<const TCHAR*>(d.getPtr());

    if (d->wDriverOffset)
        driver = i + d->wDriverOffset;

    if (d->wDeviceOffset)
        device = i + d->wDeviceOffset;

    if (d->wOutputOffset)
        output = i + d->wOutputOffset;
}


}
