/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Canvas.AdjustMarkerInfo;

namespace Canvas
{

export struct AdjustMarkerInfo
{
    bool left = true;
    bool right = true;
    bool up = true;
    bool down = true;

    using AMI = AdjustMarkerInfo;

    static AMI LeftUp()
    {
        return { 1, 0, 1, 0 };
    }
    static AMI LeftDown()
    {
        return { 1, 0, 0, 1 };
    }
    static AMI RightUp()
    {
        return { 0, 1, 1, 0 };
    }
    static AMI RightDown()
    {
        return { 0, 1, 0, 1 };
    }
    static AMI Horiz()
    {
        return { 1, 1, 0, 0 };
    }
    static AMI Vert()
    {
        return { 0, 0, 1, 1 };
    }
    static AMI Left()
    {
        return { 1, 0, 0, 0 };
    }
    static AMI Right()
    {
        return { 0, 1, 0, 0 };
    }
    static AMI Up()
    {
        return { 0, 0, 1, 0 };
    }
    static AMI Down()
    {
        return { 0, 0, 0, 1 };
    }
};

}
