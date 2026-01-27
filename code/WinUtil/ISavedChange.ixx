/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.ISavedChange;



namespace WinUtil
{

export class ISavedChange
{
public:
    virtual void lastSavedAt(const _FILETIME&) = 0;

protected:
    ~ISavedChange() = default;
};

}
