/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil:GlobalOwner;

import d1.wintypes;


namespace WinUtil
{

export class GlobalOwner
{
    d1::HGLOBAL itsGlob = {}; // ownership

    static d1::HGLOBAL Copy(d1::HGLOBAL g);

public:
    GlobalOwner() {}

    explicit GlobalOwner(d1::HGLOBAL g):
        itsGlob{ g } {}
    // Takes ownership of g

    GlobalOwner(const GlobalOwner& g):
        itsGlob{ Copy(g.itsGlob) } {}
    // Copies g

    ~GlobalOwner() { Reset(); }

    GlobalOwner& operator=(const GlobalOwner& g);
    // Frees itsGlob and copies g

    GlobalOwner& operator=(d1::HGLOBAL g);
    // Frees itsGlob and takes ownership of g

    d1::HGLOBAL Get() const { return itsGlob; }

    d1::HGLOBAL Release();
    // Releases ownership and sets itsGlob = 0;

    void Reset();
    // Frees itsGlob.
};

}
