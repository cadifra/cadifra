/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module App.RegistryAccessProvider;

import WinUtil.Registry;


namespace App
{

export class IRegistryAccessProvider
{
public:
    // may throw WinUtil::Registry::Exception

    virtual auto getUserSettingsKeyForReading() const
        -> WinUtil::Registry::Key = 0;

    virtual auto getUserSettingsKeyForWriting() const
        -> WinUtil::Registry::Key = 0;

protected:
    ~IRegistryAccessProvider() = default;
};

}
