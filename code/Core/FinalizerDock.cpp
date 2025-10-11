/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;


namespace Core
{

namespace
{

class Imp: public FinalizerDock
{
    std::vector<std::unique_ptr<Finalizer>> v_;

public:
    //-- FinalizerDock

    void Add(std::unique_ptr<Finalizer>) final;
    void ExecuteAll(Env&) final;

    //--
};

}


auto FinalizerDock::Get() -> FinalizerDock&
{
    static Imp imp;
    return imp;
}


void Imp::Add(std::unique_ptr<Finalizer> f)
{
    v_.push_back(std::move(f));
}


void Imp::ExecuteAll(Env& e)
{
    auto a = [&](const auto& f) { f->Execute(e); };
    std::ranges::for_each(v_, a);
}

}

