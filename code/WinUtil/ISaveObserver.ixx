/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.ISaveObserver;



namespace WinUtil
{

export class ISaveObserver
{
public:
    virtual void LastSavedAt(const _FILETIME&) = 0;

protected:
    ~ISaveObserver() = default;
};

}
