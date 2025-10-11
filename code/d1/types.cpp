/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module d1.types;

namespace d1
{

namespace
{

void dummy_function_doing_only_compile_time_checks()
{
    static_assert(sizeof(int8) == 1);
    static_assert(sizeof(uint8) == 1);

    static_assert(sizeof(int16) == 2);
    static_assert(sizeof(uint16) == 2);

    static_assert(sizeof(int32) == 4);
    static_assert(sizeof(uint32) == 4);

    static_assert(sizeof(float64) == 8);
}

}

}
