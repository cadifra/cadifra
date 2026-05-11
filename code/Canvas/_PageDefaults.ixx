/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Canvas.PageDefaults;

import d1.Margins;


export namespace Canvas::PageDefaults
{

constexpr d1::int32 Size = 1; // DMPAPER_LETTER;
constexpr d1::int32 Width = 21590;
constexpr d1::int32 Height = 27940;
constexpr bool Landscape = false;

constexpr d1::int32 Rows = 1;
constexpr d1::int32 Columns = 1;

constexpr d1::Margins
    Margins{ 1016, 1016, 1016, 1016 },
    CutGlueMargins{ 1016, 1016, 1016, 1016 };

constexpr d1::float64 Scale = 1.0;

}
