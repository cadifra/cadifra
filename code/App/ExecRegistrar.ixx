/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module App.ExecRegistrar;

import App.Command;
import App.CmdState;

import WinUtil.Messages;

import std;


namespace App
{

template <class T>
using Exec = void (T::*)(const WinUtil::WM_COMMAND_Msg&);

template <class T>
using Prep = bool (T::*)(const WinUtil::WM_COMMAND_Msg&);


template <class T>
class Command: public ICommand
{
    Exec<T> execute_;
    T& target_;

public:
    Command(int id, T& target, Exec<T> execute):
        ICommand{ id },
        execute_{ execute },
        target_{ target }
    {
    }

    bool prepareImp(const WinUtil::WM_COMMAND_Msg& msg) override
    {
        return true;
    }

    void executeImp(const WinUtil::WM_COMMAND_Msg& msg) override
    {
        std::invoke(execute_, target_, msg);
    }
};


template <class T>
class CommandP: public ICommand
{
    Exec<T> execute_;
    Prep<T> prepare_;
    T& target_;

public:
    CommandP(int id, T& target, Exec<T> execute, Prep<T> prepare):
        ICommand{ id },
        execute_{ execute },
        prepare_{ prepare },
        target_{ target }
    {
    }

    bool prepareImp(const WinUtil::WM_COMMAND_Msg& msg) override
    {
        return std::invoke(prepare_, target_, msg);
    }

    void executeImp(const WinUtil::WM_COMMAND_Msg& msg) override
    {
        std::invoke(execute_, target_, msg);
    }
};


export class ExecRegistrar
{
public:
    ExecRegistrar(ICommander&, WinUtil::IExceptionHandler* eh);
    ExecRegistrar(const ExecRegistrar&) = delete;
    ExecRegistrar& operator=(const ExecRegistrar&) = delete;
    ~ExecRegistrar(); // intentionally not virtual

    void enable(int id, bool);
    bool isEnabled(int id) const;

    void setState(int id, CmdState);

    template <class T>
    auto helper(T& target);

private:
    void add(std::unique_ptr<ICommand>);

    class Impl;
    std::unique_ptr<Impl> impl_;
};


template <class T>
auto ExecRegistrar::helper(T& target)
{
    class HelperType
    {
        ExecRegistrar& registrar_;
        T& target_;

    public:
        HelperType(ExecRegistrar& r, T& target):
            registrar_{ r }, target_{ target }
        {
        }

        void add(int id, Exec<T> execute)
        {
            registrar_.add(
                std::make_unique<Command<T>>(id, target_, execute));
        }

        void add(int id, Exec<T> execute, Prep<T> prepare)
        {
            registrar_.add(
                std::make_unique<CommandP<T>>(id, target_, execute, prepare));
        }
    };

    return HelperType{ *this, target };
}

}


module : private;


namespace App
{

class ExecRegistrar::Impl
{
    std::map<int, std::unique_ptr<ICommand>> plugs_;

    ICommander& commander_;
    WinUtil::IExceptionHandler* const exceptionHandler_; // may be zero, no ownership

public:
    Impl(ICommander& c, WinUtil::IExceptionHandler* eh):
        commander_{ c }, exceptionHandler_{ eh }
    {
    }

    void add(std::unique_ptr<ICommand>);

    void enable(int id, bool);
    bool isEnabled(int id) const;

    void setState(int id, CmdState);
};


void ExecRegistrar::Impl::add(std::unique_ptr<ICommand> p)
{
    auto* const orig_plug = p.get();

    const int id = p->getId();

    const bool inserted =
        plugs_.insert(std::pair{ id, std::move(p) })
            .second;

    if (not inserted)
    {
        D1_ASSERT(0);
        return;
    }

    // ## ownership transferred, p is now invalid

    orig_plug->setExceptionHandler(exceptionHandler_);
    orig_plug->set(commander_);
}


void ExecRegistrar::Impl::enable(int id, bool enable)
{
    auto i = plugs_.find(id);

    D1_ASSERT(i != end(plugs_));

    i->second->enable(enable);
}


bool ExecRegistrar::Impl::isEnabled(int id) const
{
    auto i = plugs_.find(id);

    D1_ASSERT(i != end(plugs_));

    return i->second->isEnabled();
}


void ExecRegistrar::Impl::setState(int id, CmdState s)
{
    commander_.setState(id, s);
}


ExecRegistrar::ExecRegistrar(ICommander& c, WinUtil::IExceptionHandler* eh):
    impl_{ std::make_unique<Impl>(c, eh) }
{
}


ExecRegistrar::~ExecRegistrar()
{
}


void ExecRegistrar::add(std::unique_ptr<ICommand> e)
{
    impl_->add(std::move(e));
}


void ExecRegistrar::enable(int id, bool enable)
{
    impl_->enable(id, enable);
}

bool ExecRegistrar::isEnabled(int id) const
{
    return impl_->isEnabled(id);
}


void ExecRegistrar::setState(int id, CmdState s)
{
    impl_->setState(id, s);
}

}
