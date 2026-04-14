/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;
import :Transaction;


namespace Core
{

namespace
{

class Imp: public Finalizer::Dock
{
    std::vector<std::unique_ptr<Finalizer>> v_;

public:
    //-- Finalizer::Dock

    void add(std::unique_ptr<Finalizer>) final;
    void executeAll(Env&) final;

    //--
};

}


auto Finalizer::getDock() -> Dock&
{
    static Imp imp;
    return imp;
}


void Imp::add(std::unique_ptr<Finalizer> f)
{
    v_.push_back(std::move(f));
}


void Imp::executeAll(Env& e)
{
    auto a = [&](const auto& f) { f->execute(e); };
    std::ranges::for_each(v_, a);
}

}

