/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include <Windows.h>

module GraphUtil.Metafile;

import d1.types;
import d1.WideFileNameFStream;

import std;


namespace
{

constexpr d1::uint16 HMperInch = 2540;
constexpr d1::uint32 APMHEADER_KEY = 0x9AC6CDD7l;

#pragma pack(push, 1)
struct APMFILEHEADER // Aldus placeable metafile header
{
    d1::uint32 key;
    d1::uint16 hmf;
    d1::int16 left;
    d1::int16 top;
    d1::int16 right;
    d1::int16 bottom;
    d1::uint16 inch;
    d1::uint32 reserved;
    d1::uint16 checksum;
};
#pragma pack(pop)


d1::uint16 calculateAPMCheckSum(const APMFILEHEADER& ah)
{
    d1::uint16 checksum = 0;
    const d1::uint16* i = reinterpret_cast<const d1::uint16*>(&ah);

    for (int count = 0; count < 10; ++count, ++i)
        checksum ^= *i;

    return checksum;
}

}


namespace GraphUtil
{

namespace PlaceableMetafile // Aldus placeable metafile format
{


void write(const METAFILEPICT& mp, const std::wstring& filename)
{
    auto ah = APMFILEHEADER{
        .key = APMHEADER_KEY,
        .hmf = 0,
        .left = 0,
        .top = 0,
        .right = static_cast<d1::int16>(mp.xExt),
        .bottom = static_cast<d1::int16>(mp.yExt),
        .inch = HMperInch,
        .reserved = 0,
        .checksum = 0
    };

    ah.checksum = calculateAPMCheckSum(ah);

    auto os = d1::WideFileNameFStream<std::ostream>{ filename.c_str(), L"wb" };

    os.write(reinterpret_cast<const char*>(&ah), sizeof(ah));

    UINT size = ::GetMetaFileBitsEx(mp.hMF, 0, 0);

    auto buf = std::vector<char>(size);

    size = ::GetMetaFileBitsEx(mp.hMF, (UINT)buf.size(), buf.data());
    D1_ASSERT(size == buf.size());

    os.write(buf.data(), buf.size());
}


}
}
