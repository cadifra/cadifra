export module Canvas.PageInfo;

import Canvas.PageDefaults;

import d1.Size;
import d1.Margins;


namespace Canvas
{

export class PageInfo
{
public:
    d1::fSize Size = { PageDefaults::Width, PageDefaults::Height };
    bool Landscape = PageDefaults::Landscape;
    d1::float64 Scale = PageDefaults::Scale;
    d1::int32 Rows = PageDefaults::Rows;
    d1::int32 Cols = PageDefaults::Columns;
    d1::fMargins Margins = PageDefaults::Margins;
    d1::fMargins CutGlueMargins = PageDefaults::CutGlueMargins;
};

}
