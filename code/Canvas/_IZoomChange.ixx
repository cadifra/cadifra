/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Canvas.IZoomChange;

import d1.Point;


namespace Canvas
{

export class IZoomChange
{
public:
    virtual void zoomNotification(const double& zoomFactor,
        const d1::Point& fixPoint) = 0;
    // The "fixPoint" is in client coordinates an marks the point that
    // didn't change its position during the change of the zoom factor.

protected:
    ~IZoomChange() = default;
};

}
