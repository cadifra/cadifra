/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Canvas.PrinterCanvas;

import Canvas.Canvas;

import std;


namespace Canvas
{

export class IAbort
{
public:
    virtual bool abort() const = 0;
    // Return true to abort the function that got this interface
    // as a parameter.

protected:
    ~IAbort() = default;
};


export class IPrinterCanvas: public virtual Canvas
{
public:
    virtual void print(
        const IAbort&,
        bool fitToPage,
        bool glueCutMarks,
        const std::pair<int, int>& printRange, // "from" page, "to" page
        const std::wstring& destination,       // "" or a filename,
        bool evaluationMode) = 0;
};

}
