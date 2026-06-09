/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module Editor.MetafileEvalString;

import Canvas.Canvas;
import Canvas.Group;

namespace Editor
{

export class MetafileEvalString
{
    Canvas::Group custom_;

public:
    MetafileEvalString(Canvas::Canvas& c);
};

}


module : private;

import Editor.LicenseInfo;

import Canvas.ICustomDrawer;

import d1.Rect;
import d1.round;

import Resources;


namespace Editor
{

constexpr d1::float64 PointToCanvas = 2540.0 / 72.0; // [canvas units/points]


class CustomDrawer: public Canvas::ICustomDrawer
{
    d1::fnRect bounds_;

public:
    CustomDrawer(const d1::fnRect& bounds):
        bounds_{ bounds }
    {
    }

    //-- Canvas::ICustomDrawer

    void doDraw(Canvas::IDeviceContext&) final {}
    void doDraw(Canvas::IDeviceContextStatic& dc) final;
    d1::fnRect getBounds() final { return bounds_; }

    //--
};


void CustomDrawer::doDraw(Canvas::IDeviceContextStatic& dc)
{
    static const auto AppName = std::wstring{ Resources::EvaluationStrings::appName() };
    static const auto URL = std::wstring{ Resources::EvaluationStrings::URL() };

    dc.setTextAlign(TA_BOTTOM | TA_RIGHT);
    dc.setTextColor(RGB(150, 150, 150));

    const auto offset = dc.offset();
    const auto scale = dc.scale();

    auto lf = LOGFONT{
        .lfHeight = -d1::round(8.0 * std::abs(scale.y) * PointToCanvas),
        .lfWidth = 0,
        .lfEscapement = 0,
        .lfOrientation = 0,
        .lfWeight = FW_BOLD,
        .lfItalic = FALSE,
        .lfUnderline = FALSE,
        .lfStrikeOut = FALSE,
        .lfCharSet = ANSI_CHARSET,
        .lfOutPrecision = OUT_TT_ONLY_PRECIS,
        .lfClipPrecision = CLIP_DEFAULT_PRECIS,
        .lfQuality = DEFAULT_QUALITY,
        .lfPitchAndFamily = DEFAULT_PITCH,
        .lfFaceName = {}
    };
    wcsncpy(lf.lfFaceName, L"Arial", LF_FACESIZE - 1);
    dc.selectFont(lf);

    const auto p = d1::Point{
        d1::round(scale.x * bounds_.r + offset.dx),
        d1::round(scale.y * bounds_.b + offset.dy)
    };

    const int line = d1::round(-lf.lfHeight * 1.2);
    HDC hdc = dc.dangerousAccess();

    D1_VERIFY(
        ::TextOut(
            hdc, p.x, p.y - line, AppName.c_str(),
            static_cast<int>(AppName.size())));
    D1_VERIFY(
        ::TextOut(
            hdc, p.x, p.y, URL.c_str(),
            static_cast<int>(URL.size())));
}


MetafileEvalString::MetafileEvalString(Canvas::Canvas& c)
{
    if (not LicenseInfo::instance().evaluationMode())
        return;
    custom_ = {};
    c.custom(custom_, std::make_unique<CustomDrawer>(c.getLogicBounds()));
}

}
