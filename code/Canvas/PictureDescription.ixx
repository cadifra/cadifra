/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Canvas.PictureDescription;

namespace Canvas
{

export class PictureDescription
{
    int resID_ = 0;

public:
    PictureDescription(int ResID):
        resID_{ ResID }
    {
    }
    PictureDescription() {}

    int getID() const { return resID_; }

    // uses compiler generated copy and assignment operator
};

}
