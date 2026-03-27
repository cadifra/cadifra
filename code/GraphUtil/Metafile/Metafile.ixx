/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module GraphUtil.Metafile;

import WinUtil.types;

import std;


namespace GraphUtil
{

namespace PlaceableMetafile // Aldus placeable metafile format
{
export void write(const WinUtil::METAFILEPICT&, const std::wstring& filename);
}


}
