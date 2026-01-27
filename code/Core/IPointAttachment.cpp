/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;


namespace Core
{

auto IPointAttachment::findNearestPointImpl(const d1::fPoint&, bool) const
    -> NearestRes
{
    return { pos(), {} };
}

}
