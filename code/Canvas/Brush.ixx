/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>
#include <gdiplus.h>

#include "d1/d1assert.h"

export module Canvas.Brush;

import Canvas.Color;

import d1.Observer;
import d1.types;

import std;


namespace Canvas
{

export class Brush
{
public:
    enum class Type
    {
        HOLLOW,
        SOLID
    };

    Brush() {}
    Brush(Type, Color = {});

    // uses compiler generated assignment operator and copy constructor

    bool operator==(const Brush&) const = default;
    bool operator<(const Brush&) const; // allows sorting

    auto getType() const -> Type { return type_; }
    auto getColor() const -> Color { return color_; }

private:
    Type type_ = Type::HOLLOW;
    Color color_ = Color::WINDOWTEXT;
};


inline Brush::Brush(Type t, Color c):
    type_{ t }, color_{ c }
{
    if (type_ == Type::HOLLOW)
        color_ = Color{};
}

inline bool Brush::operator<(const Brush& b) const
{
    if (type_ != b.type_)
        return type_ < b.type_;

    return color_ < b.color_;
}

}
