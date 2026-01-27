/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.Exception;

import std;


namespace d1
{

export class Exception: public std::exception
{
    bool handled_ = false;

public:
    bool handled() const { return handled_; }
    void setHandled() { handled_ = true; }

protected:
    ~Exception() = default;
};

}
