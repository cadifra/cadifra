#pragma once
/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */


#define D1_DEFINE_EXCEPTION(namespace_, base, name) \
    class name: public base                         \
    {                                               \
    public:                                         \
        const char* what() const                    \
        {                                           \
            return #namespace_ "::" #name;          \
        }                                           \
    };
