/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module Editor.NewWindowPlacement;


namespace Editor
{

export namespace NewWindowPlacement
{

void set(HWND w, bool closing);
// Sets the coordinates that will be used as the position for new
// windows.
// "closing=true ": the position of w will be recycled.
// "closing=false": the position for a new window will be the position
//                  of w plus an offset.

bool New(HWND w);
// Calculates a position for a new window depending on the parameters
// from the last Set call or from the initialization from the registry.
// Applies this position to the window w.
// Returns false if Set was never called or there where no values found
// in the registry.

void storeToRegistry();
void initFromRegistry();

}

}


module : private;


import Editor.Installer;

import WinUtil.WindowPlacement;


namespace Editor
{

const wchar_t* theRegistryName = L"WindowPlacement";
HWND theActualWindow = 0;

auto theWindowPlacementHandler = WinUtil::WindowPlacementHandler{};


namespace NewWindowPlacement
{

void set(HWND w, bool closing)
{
    if (closing)
    {
        theActualWindow = 0;
        theWindowPlacementHandler.getFrom(w);
    }
    else
    {
        theActualWindow = w;
        theWindowPlacementHandler = WinUtil::WindowPlacementHandler{};
    }
}


bool New(HWND w)
{
    if (theActualWindow and ::IsWindow(theActualWindow))
    {
        auto wph = WinUtil::WindowPlacementHandler{
            WinUtil::calcNewWindowPos(theActualWindow)
        };
        wph.setTo(w);
        return true;
    }

    if (theWindowPlacementHandler.ok())
    {
        theWindowPlacementHandler.setTo(w);
        return true;
    }

    return false;
}


void storeToRegistry()
{
    try
    {
        auto k = Installer::instance().getUserSettingsKey(true);

        theWindowPlacementHandler.write(k, theRegistryName);
    }
    catch (WinUtil::Registry::Exception)
    {
    }
}


void initFromRegistry()
{
    theActualWindow = 0;

    try
    {
        auto k = Installer::instance().getUserSettingsKey(false);

        theWindowPlacementHandler.read(k, theRegistryName);
    }
    catch (WinUtil::Registry::Exception)
    {
    }
}

}

}
