/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module App.NewWindowDefaultsProvider;

namespace App
{

export class NewWindowDefaultsProvider
{
public:
    NewWindowDefaultsProvider() = default;
    virtual ~NewWindowDefaultsProvider() = 0;

    NewWindowDefaultsProvider(const NewWindowDefaultsProvider&) = delete;
    NewWindowDefaultsProvider& operator=(const NewWindowDefaultsProvider&) = delete;
};


inline NewWindowDefaultsProvider::~NewWindowDefaultsProvider() = default;

}
