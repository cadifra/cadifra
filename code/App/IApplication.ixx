/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module App.IApplication;

import WinUtil.MessageLoop;
import WinUtil.types;


namespace App
{

export class IApplication
{
public:
    virtual auto getMessageLoop() -> WinUtil::MessageLoop& = 0;

    virtual WinUtil::HINSTANCE getInstanceHandle() const = 0;

protected:
    ~IApplication() = default;
};

}
