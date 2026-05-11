/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Canvas.Canvas;

import d1.Rect;


namespace Canvas
{


void addBoxAdjustMarkers(Canvas& c, Group& g, const d1::fnRect& r, bool isTarget)
{
    using AMI = AdjustMarkerInfo;

    c.adjustMarker(g, r.topLeft(), AMI::LeftUp(), isTarget);
    c.adjustMarker(g, r.topRight(), AMI::RightUp(), isTarget);
    c.adjustMarker(g, r.bottomLeft(), AMI::LeftDown(), isTarget);
    c.adjustMarker(g, r.bottomRight(), AMI::RightDown(), isTarget);
}


void addBoxMidPointsAdjustMarkers(Canvas& c, Group& g, const d1::fnRect& r, bool isTarget)
{
    using AMI = AdjustMarkerInfo;

    auto A = d1::point(r.l + r.width() / 2.0, r.t);
    auto B = d1::point(A.x, r.b);
    auto C = d1::point(r.l, r.t - r.height() / 2.0);
    auto D = d1::point(r.r, C.y);

    c.adjustMarker(g, A, AMI::Horiz(), isTarget);
    c.adjustMarker(g, B, AMI::Horiz(), isTarget);
    c.adjustMarker(g, C, AMI::Vert(), isTarget);
    c.adjustMarker(g, D, AMI::Vert(), isTarget);
}

}
