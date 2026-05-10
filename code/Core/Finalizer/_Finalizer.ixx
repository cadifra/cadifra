/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Finalizer;

import Core.Main;

import std;

namespace Core
{

export class Finalizer
{
public:
    Finalizer() = default;
    virtual ~Finalizer() = default;

    Finalizer(const Finalizer&) = delete;
    Finalizer& operator=(const Finalizer& rhs) = delete;

    virtual void execute(Env&) = 0;

    class Dock;
    static auto getDock() -> Dock&;
};


class Finalizer::Dock
{
public:
    virtual void add(std::unique_ptr<Finalizer>) = 0;

    virtual void executeAll(Env&) = 0;

protected:
    ~Dock() = default;
};

}
