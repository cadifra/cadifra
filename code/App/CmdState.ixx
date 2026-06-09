/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module App.CmdState;

namespace App
{

export class CmdState
{
public:
    enum class T
    {
        unchecked,
        checked,
        radio_checked
    };

    CmdState(T t = T::unchecked):
        t{ t }
    {
    }

    // uses compiler generated assignment operator, copy constructor
    // and destructor

    operator T() const { return t; }

private:
    T t;
};

}
