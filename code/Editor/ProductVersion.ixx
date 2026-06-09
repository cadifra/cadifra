/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "main/build-config.h"

export module Editor.ProductVersion;

import WinUtil.ProductVersion;


namespace Editor::ProductVersion
{

export auto get() -> const WinUtil::ProductVersion&
{
    static auto pv = WinUtil::ProductVersion{
        D1_MAIN_BUILD_CONFIG_PRODUCT_VERSION_FIRST,
        D1_MAIN_BUILD_CONFIG_PRODUCT_VERSION_SECOND,
        D1_MAIN_BUILD_CONFIG_PRODUCT_VERSION_THIRD
    };

    return pv;
}

}
