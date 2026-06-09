/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:VIPointable;

import Core.Weight;

import d1.Point;


namespace Core
{

export
{
class IView;
}

export class VIPointable // mouse pointable object in a view
{
public:
    virtual auto getView() const -> IView& = 0;

    virtual bool checkHit(const d1::Point& pos, d1::int32 distance,
        bool attaching) const = 0;
    // returns true, if this VIPointable feels it is hit, given that
    // the mouse points to position pos within distance.

    virtual auto getWeight(const d1::Point& pos, d1::int32 distance) const -> Weight = 0;
    // Used in IView::findPointable(). Usually, the model is consulted to compute
    // the Weight [see IElement::getWeight()].

    virtual void setCursor(const d1::Point& mouse_pos) const = 0;

protected:
    ~VIPointable() = default;
};

}
