/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core:Interfaces;

import d1.Rect;

import :Element;

import std;


namespace Core
{

export class ICheckable
{
public:
    virtual void check() const = 0;

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
    virtual void setName(Env&, const std::wstring&) = 0;
    virtual std::wstring getName() const = 0;
};


export class IPosition
{
public:
    virtual d1::fPoint pos() const = 0;
    virtual d1::fPoint oldPos() const = 0; // pos before transaction started

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
    virtual void resize(Env&, const d1::nRect& /*new_rect*/, const ResizeMode* = 0) = 0;

    virtual d1::Point pos() const = 0;
    virtual d1::Point oldPos() const = 0;

    virtual void setPosition(Env&, const d1::Point&) = 0;

    virtual d1::Size size() const = 0;
    virtual d1::Size oldSize() const = 0;

    virtual d1::int32 minimalWidth() const { return MinSizeX; }
    virtual d1::int32 minimalHeight() const { return MinSizeY; }

    virtual double ratio() const
    {
        return std::max<double>(size().h, minimalHeight()) /
               std::max<double>(size().w, minimalWidth());
    }
};


export template <class T>
class IPartner: public virtual IElement
{
public:
    virtual void add(Env&, T&) = 0;
    virtual void forget(Env&, const T&) = 0;
    virtual bool has(const T&) const = 0;
    virtual void initialAdd(Env&, T&) = 0;
};


export class IDumpable
{
public:
    virtual void dump(std::wostream&) const = 0;

    friend auto operator<<(std::wostream& s, const IDumpable& d) -> std::wostream&
    {
        d.dump(s);
        return s;
    }

    friend auto operator<<(std::ostream& s, const IDumpable& d) -> std::ostream&;

protected:
    ~IDumpable() = default;
};

}
