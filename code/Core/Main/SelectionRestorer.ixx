/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:SelectionRestorer;

import std;


namespace Core
{

export class IView;


namespace Selection
{

export class Tracker;


export class IRestorer
{
public:
    IRestorer() = default;

    virtual ~IRestorer() = default;

    IRestorer(const IRestorer&) = delete;
    IRestorer& operator=(const IRestorer&) = delete;

    virtual void restore(Tracker&, IView&) = 0;
};

export using IRestorerRef = std::shared_ptr<IRestorer>;

}

}
