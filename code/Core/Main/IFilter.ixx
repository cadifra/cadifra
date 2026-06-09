/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:IFilter;

import std;


namespace Core
{

export
{
class VIPointable;
}


export class IFilter
{
public:
    virtual bool pass(const VIPointable&) const = 0;

    IFilter() = default;

    virtual ~IFilter() = default;

    IFilter(const IFilter&) = delete;
    IFilter& operator=(const IFilter&) = delete;
};

export using FilterRef = std::shared_ptr<IFilter>;

}
