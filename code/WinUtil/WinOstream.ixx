/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module WinUtil.WinOstream;

import std;


export
{

std::ostream& operator<<(std::ostream&, const GUID&);
std::ostream& operator<<(std::ostream&, const std::wstring);
std::ostream& operator<<(std::ostream&, const FORMATETC&);
std::ostream& operator<<(std::ostream&, const DVTARGETDEVICE&);
std::ostream& operator<<(std::ostream&, const STGMEDIUM&);
std::ostream& operator<<(std::ostream&, const DATADIR&);
std::ostream& operator<<(std::ostream&, const OLEVERB&);
std::ostream& operator<<(std::ostream&, const POINT&);
std::ostream& operator<<(std::ostream&, const RECT&);
std::ostream& operator<<(std::ostream&, const RECT*);
std::ostream& operator<<(std::ostream&, const SIZE&);
std::ostream& operator<<(std::ostream&, LPCOLESTR&);
std::ostream& operator<<(std::ostream&, HRGN);


class Printer
{
    virtual void print(std::ostream&) const = 0;
    friend std::ostream& operator<<(std::ostream&, const Printer&);
};

inline std::ostream& operator<<(std::ostream& os, const Printer& p)
{
    p.print(os);
    return os;
}



class PrintCLIPFORMAT: public Printer
{
    CLIPFORMAT m_;
    virtual void print(std::ostream&) const;

public:
    PrintCLIPFORMAT(CLIPFORMAT cf):
        m_{ cf }
    {
    }
};


class PrintTYMED: public Printer
{
    DWORD m_;
    virtual void print(std::ostream&) const;

public:
    PrintTYMED(DWORD t):
        m_{ t }
    {
    }
};


class PrintADVF: public Printer
{
    DWORD m_;
    virtual void print(std::ostream&) const;

public:
    PrintADVF(DWORD t):
        m_{ t }
    {
    }
};


class PrintHRESULT: public Printer
{
    HRESULT m_;
    virtual void print(std::ostream&) const;

public:
    PrintHRESULT(HRESULT t):
        m_{ t }
    {
    }
};

}
