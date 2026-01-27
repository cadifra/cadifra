/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil.IReadOnlyStateChange;


namespace WinUtil
{

export class IReadOnlyStateChange
{
public:
    virtual void readOnlyStateChanged(bool IsReadOnly) = 0;

protected:
    ~IReadOnlyStateChange() = default;
};

}
