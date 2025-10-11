/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.Exception;

import std;


namespace d1
{

export class Exception: public std::exception
{
    bool isHandled = false;

public:
    bool Handled() const { return isHandled; }
    void SetHandled() { isHandled = true; }

protected:
    ~Exception() = default;
};

}
