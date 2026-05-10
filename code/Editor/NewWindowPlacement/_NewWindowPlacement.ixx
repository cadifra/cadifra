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
