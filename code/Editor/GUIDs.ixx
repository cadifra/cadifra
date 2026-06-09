/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "main/installer-config.h"
#include "main/LGUID.h"

export module Editor.GUIDs;

import d1.wintypes;

namespace Editor
{

export struct GUIDs
{
    static auto diagram() -> const d1::CLSID&;
};

}

module : private;

import WinUtil.CLSID;


namespace Editor
{

auto GUIDs::diagram() -> const CLSID&
{
    static CLSID id =
        WinUtil::convertToCLSID(
            D1_MAIN_LGUID(
                D1_MAIN_INSTALLER_CONFIG_DIAGRAM_CLSID));
    return id;
}

}
