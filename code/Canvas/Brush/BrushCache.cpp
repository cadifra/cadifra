/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>
#include <gdiplus.h>

module Canvas.Brush;

import d1.Observer;

import WinUtil.Gdi;

import std;


namespace Canvas
{

namespace
{
using C = BrushCache;
}


class C::Imp:
    public std::enable_shared_from_this<Imp>,
    private ColorCache::IObserver
{
    using Table = std::map<Brush, Imp*>;

    const Brush brush_;
    WinUtil::GdiObjectOwner<HBRUSH> brushHandle_;
    std::unique_ptr<Gdiplus::Brush> gdiplusBrush_;
    ColorCache colorCache_;
    d1::Observer::C<ColorCache::IObserver> colorCacheConnector_;

    static auto theirTable() -> Table&;
    static HBRUSH createHandle(const Brush&);

    //-- ColorCache::IObserver

    void systemColorsChanged() final;

public:
    Imp(const Brush&, Table&);
    virtual ~Imp();

    static auto create(const Brush&) -> std::shared_ptr<Imp>;
    HBRUSH getHBRUSH() { return brushHandle_.get(); }
    auto getGdiplusBrush() -> Gdiplus::Brush*;
};


auto C::Imp::theirTable() -> Table&
{
    static Table singleton;
    return singleton;
}


auto C::Imp::create(const Brush& b)
    -> std::shared_ptr<Imp>
{
    auto& t = theirTable();
    auto i = t.find(b);
    if (i != end(t))
        return i->second->shared_from_this();
    else
        return std::make_shared<Imp>(b, t);
}


C::Imp::Imp(const Brush& b, Table& t):
    brush_{ b },
    brushHandle_{ createHandle(b) },
    colorCacheConnector_{ colorCache_.getConnector() }
{
    t[brush_] = this;
    colorCacheConnector_.connect(*this);
}


C::Imp::~Imp()
{
    theirTable().erase(brush_);
}


HBRUSH C::Imp::createHandle(const Brush& b)
{
    if (b.getType() == Brush::Type::HOLLOW)
        return static_cast<HBRUSH>(::GetStockObject(HOLLOW_BRUSH));

    if (b.getType() == Brush::Type::SOLID)
    {
        const ::Canvas::Color c = b.getColor();

        if (c.isAutomatic())
            return ::GetSysColorBrush(c.getAutomaticColor());

        if (c == Color::Black)
            return static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));

        if (c == Color::White)
            return static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));

        return ::CreateSolidBrush(c.getRGB());
    }

    return 0;
}


auto C::Imp::getGdiplusBrush() -> Gdiplus::Brush*
{
    if (gdiplusBrush_.get())
        return gdiplusBrush_.get();

    if (brush_.getType() == Brush::Type::HOLLOW)
        return nullptr;

    auto c = colorCache_.getColor(brush_.getColor()).getCOLORREF();

    gdiplusBrush_ = std::make_unique<Gdiplus::SolidBrush>(
        Gdiplus::Color{ GetRValue(c), GetGValue(c), GetBValue(c) });

    return gdiplusBrush_.get();
}


void C::Imp::systemColorsChanged()
{
    gdiplusBrush_.reset();
}


C::BrushCache(const std::shared_ptr<Imp>& imp):
    imp_{ imp }
{
}


C::BrushCache(const BrushCache& b):
    imp_{ b.imp_ }
{
}


C::~BrushCache() = default;


BrushCache& C::operator=(const BrushCache& b)
{
    imp_ = b.imp_;
    return *this;
}


HBRUSH C::getHBRUSH() const
{
    return imp_->getHBRUSH();
}


auto C::getGdiplusBrush() const -> Gdiplus::Brush*
{
    return imp_->getGdiplusBrush();
}


bool C::operator==(const BrushCache& b) const
{
    return imp_->getHBRUSH() == b.imp_->getHBRUSH();
}


auto C::create(const Brush& b) -> BrushCache
{
    return BrushCache{ Imp::create(b) };
}


auto C::Default() -> BrushCache
{
    static auto bc = BrushCache{ Imp::create(Brush{}) };
    return bc;
}

}
