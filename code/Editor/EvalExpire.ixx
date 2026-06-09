/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Editor.EvalExpire;

import std;


namespace Editor
{

export class EvalExpire
{
public:
    static auto instance() -> EvalExpire&; // singleton

    ~EvalExpire();

    int remainingDays() const;

    constexpr static int MaxDays = 30;

private:
    EvalExpire();

    class Imp;
    std::unique_ptr<Imp> imp_;
};

}
