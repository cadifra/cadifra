/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil:IReadOnlyObserver;


namespace WinUtil
{

export class IReadOnlyObserver
{
public:
    virtual void ReadOnlyStateChanged(bool IsReadOnly) = 0;

protected:
    ~IReadOnlyObserver() = default;
};

}
