/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:Weight;

import d1.Rect;

import std;


namespace Core
{

export class IElement;
export class IView;


export class Weight
//
//  A weight which is attached to model and view elements.
//  An element with a higher weight is preferred in function
//  IView::FindPointable.
//  "Higher" means in terms of its operator>().
//
{
    class Impl;
    std::shared_ptr<Impl> impl_;
    Weight(const std::shared_ptr<Impl>&);

public:
    Weight();
    ~Weight();

    Weight(const Weight&) = default;
    Weight& operator=(const Weight&) = default;

    static auto invisible() -> Weight;
    static auto text() -> Weight;

    static auto control(const d1::Point& weightAt, d1::int32 fuzziness,
        const d1::fPoint& point) -> Weight;

    static auto point(const d1::Point& weightAt, d1::int32 fuzziness,
        const d1::fPoint& point) -> Weight;

    static auto line(const d1::Point& weightAt, d1::int32 fuzziness,
        const d1::fPoint& a, const d1::fPoint& b) -> Weight;

    static auto area(const d1::Point& weightAt, d1::int32 fuzziness,
        const d1::nRect& r) -> Weight;

    void increaseSelectionBias(const IView* v, const IElement& m);
    // v may be zero (means there is no view).

    void operator+=(const Weight& rhs);

    bool operator==(const Weight& rhs) const;
    std::strong_ordering operator<=>(const Weight& rhs) const;

    void print(std::ostream& s) const;

    friend auto operator<<(std::ostream& s, const Weight& w) -> std::ostream&
    {
        w.print(s);
        return s;
    }
};


}
