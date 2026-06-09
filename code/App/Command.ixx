/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module App.Command;

import App.CmdState;
import App.ICmdView;

import WinUtil.IWindow;
import WinUtil.Messages;
import WinUtil.IExceptionHandler;

import std;


namespace App
{

export class ICommand;

export class ICommander
{
public:
    virtual void hire(ICommand&) = 0; // reference only
    virtual void fire(ICommand&) = 0;
    virtual void enableNotification(ICommand&) = 0;

    virtual void add(const std::shared_ptr<ICmdView>&) = 0;
    virtual void forget(ICmdView&) = 0;

    virtual auto getOwner() -> const WinUtil::IWindow& = 0;
    virtual void setState(int id, CmdState) = 0;

protected:
    ~ICommander() = default;
};


class ICommand
{
public:
    ICommand(int id):
        id_{ id }
    {
    }

    ICommand(const ICommand&) = delete;
    ICommand& operator=(const ICommand&) = delete;

    virtual ~ICommand();

    bool prepare(const WinUtil::WM_COMMAND_Msg&);
    void execute(const WinUtil::WM_COMMAND_Msg&);

    void enable(bool);
    bool isEnabled() const { return enabled_; }

    int getId() const { return id_; }

    void set(ICommander&);

    void setExceptionHandler(WinUtil::IExceptionHandler* eh)
    {
        exceptionHandler_ = eh;
    }

private:
    virtual bool prepareImp(const WinUtil::WM_COMMAND_Msg&) = 0;
    virtual void executeImp(const WinUtil::WM_COMMAND_Msg&) = 0;

    const int id_ = 0;
    ICommander* commander_ = nullptr;
    bool enabled_ = true;
    WinUtil::IExceptionHandler* exceptionHandler_ = nullptr; // may be zero, no ownership
};

}


module : private;


namespace App
{

ICommand::~ICommand()
{
    if (commander_)
        commander_->fire(*this);
}


void ICommand::set(ICommander& c)
{
    D1_ASSERT(commander_ == 0);
    commander_ = &c;
    commander_->hire(*this);
}


void ICommand::enable(bool e)
{
    if (e and not enabled_)
    {
        enabled_ = true;
        if (commander_)
            commander_->enableNotification(*this);
    }
    else if (not e and enabled_)
    {
        enabled_ = false;
        if (commander_)
            commander_->enableNotification(*this);
    }
}


bool ICommand::prepare(const WinUtil::WM_COMMAND_Msg& msg)
{
    if (not exceptionHandler_)
        return prepareImp(msg);

    D1_ASSERT(exceptionHandler_);

    try
    {
        return prepareImp(msg);
    }
    catch (...)
    {
        exceptionHandler_->handleException();
        throw;
    }
}


void ICommand::execute(const WinUtil::WM_COMMAND_Msg& msg)
{
    if (not exceptionHandler_)
    {
        executeImp(msg);
        return;
    }

    D1_ASSERT(exceptionHandler_);

    try
    {
        executeImp(msg);
    }
    catch (...)
    {
        exceptionHandler_->handleException();
        throw;
    }
}

}
