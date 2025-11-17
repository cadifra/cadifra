/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core:Interfaces;

import d1.Rect;

import :Base;

import std;


namespace Core
{

export class ICheckable
{
public:
    virtual void Check() const = 0;

protected:
    ~ICheckable() = default;
};


export class IDeletable: public virtual IElement
{
public:
    virtual void Delete(Env&) = 0;
};


export class INamed: public virtual IElement
{
public:
    virtual void SetName(Env&, const std::wstring&) = 0;
    virtual std::wstring GetName() const = 0;
};


export class IPosition
{
public:
    virtual d1::fPoint Pos() const = 0;
    virtual d1::fPoint OldPos() const = 0; // pos before transaction started

protected:
    ~IPosition() = default;
};


export class IResizable: public virtual IElement
{
    constexpr static int MinSizeX = 300, MinSizeY = 300;

public:
    struct ResizeMode
    {
        bool horizontal_symmetric = false;
    };
    virtual void Resize(Env&, const d1::nRect& /*new_rect*/, const ResizeMode* = 0) = 0;

    virtual d1::Point Pos() const = 0;
    virtual d1::Point OldPos() const = 0;

    virtual void SetPosition(Env&, const d1::Point&) = 0;

    virtual d1::Size Size() const = 0;
    virtual d1::Size OldSize() const = 0;

    virtual d1::int32 MinimalWidth() const { return MinSizeX; }
    virtual d1::int32 MinimalHeight() const { return MinSizeY; }

    virtual double Ratio() const
    {
        return std::max<double>(Size().h, MinimalHeight()) /
               std::max<double>(Size().w, MinimalWidth());
    }
};


export template <class T>
class IPartner: public virtual IElement
{
public:
    virtual void Add(Env&, T&) = 0;
    virtual void Forget(Env&, const T&) = 0;
    virtual bool Has(const T&) const = 0;
    virtual void InitialAdd(Env&, T&) = 0;
};


export class IDumpable
{
public:
    virtual void Dump(std::wostream&) const = 0;

    friend auto operator<<(std::wostream& s, const IDumpable& d) -> std::wostream&
    {
        d.Dump(s);
        return s;
    }

    friend auto operator<<(std::ostream& s, const IDumpable& d) -> std::ostream&;

protected:
    ~IDumpable() = default;
};

}
