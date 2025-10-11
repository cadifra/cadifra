/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.NoValueSemantics;

namespace d1
{

export class NoValueSemantics
{
public:
    NoValueSemantics() {}
    NoValueSemantics(const NoValueSemantics&) = delete;
    NoValueSemantics& operator=(const NoValueSemantics&) = delete;
};

}
