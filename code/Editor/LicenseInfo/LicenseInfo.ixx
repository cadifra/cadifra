/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module Editor.LicenseInfo;

import std;

namespace Editor
{

export class LicenseInfo
{
public:
    static LicenseInfo& instance(); // singleton
    // The first call of "Instance" starts a new thread that analyzes the
    // license file. Subsequent calls of "Instance" have no effect.

    bool evaluationMode();
    // Waits until the analyzing thread has finished.
    // Returns false if a valid license exists.

    std::string getAttribute(const std::string& attributeName);
    // Waits until the analyzing thread has finished.
    // Returns the value of the attribute "attributeName" from the license
    // file or "" if the attribute doesn't exist or there is no valid
    // license file.


    void add(HWND w);
    // The LicenseInfo module posts a LicenseInfoMsg to the window "w" as
    // soon as the license file has been analyzed (or immediately if the
    // license file was analyzed before you call add).

    void forget(HWND);

    class Adder
    {
        HWND W_;

    public:
        Adder(HWND w = 0);
        void add(HWND w);
        ~Adder();
    };


    ~LicenseInfo();
    class GFC;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;

    LicenseInfo();

    LicenseInfo(const LicenseInfo&) = delete;
    LicenseInfo& operator=(const LicenseInfo&) = delete;
};

}
