/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module App.ExecRegistrar;

import d1.wintypes;

import WinUtil.IWindow;
import WinUtil.MenuHandle;
import WinUtil.Messages;
import WinUtil.ProcRegistrar;
import WinUtil.Registry;

import std;


namespace App
{

export class ICommand;


export class CmdState
{
public:
    enum class T
    {
        unchecked,
        checked,
        radio_checked
    };

private:
    T t;

public:
    CmdState(T t = T::unchecked):
        t{ t }
    {
    }

    // uses compiler generated assignment operator, copy constructor
    // and destructor

    operator T() const { return t; }
};


export class ICmdView
{
public:
    ICmdView(int id):
        id_(id) {}
    ICmdView(const ICmdView&) = delete;
    ICmdView& operator=(const ICmdView&) = delete;

    virtual ~ICmdView() = default;

    virtual void update(bool isEnabled, CmdState) const = 0;

    int getId() const { return id_; }

private:
    int id_;
};


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


export class Commander
{
public:
    Commander(const WinUtil::IWindow&);

    Commander(const Commander&) = delete;
    Commander& operator=(const Commander&) = delete;

    ~Commander(); // intentionally not virtual

    auto get() const -> ICommander&;

private:
    class Impl;
    std::shared_ptr<Impl> impl_;
};


export class ContextMenu
{
public:
    explicit ContextMenu(d1::WORD ResourceId, int pos = 0);
    // The menu with "ResourceId" is loaded from the resources
    // and the submenu at position "pos", which has to be
    // a popup menu, will be shown whenever you call the member
    // function "Track". The loaded menu is deleted in the
    // destructor.
    // The constructor uses the submenu at position "pos",
    // because it is not possible to store a popup menu in
    // the resources which is not member of a regular menu.

    void track(int x, int y, ICommander&, bool immediate);
    // Shows the context menu at position (x,y) [in screen coordinates]
    // and let the  user select a menu command. "Track" calls via OS the
    // functions that are registered in the ICommander for
    // the selected command. "Track" returns after all
    // registered functions returned.
    // Before "Track" shows the context menu, the menu items
    // are grayed if the according command is disabled in
    // the ICommander.


    // Read and Write may throw a WinUtil::Registry::Exception
    static void readDefaults(const WinUtil::Registry::Key& k, const std::wstring& valueName);
    static void writeDefaults(const WinUtil::Registry::Key& k, const std::wstring& valueName);

private:
    WinUtil::MenuHandle menu_;
    d1::WORD resId_;

    using CmdViewVector = std::vector<std::shared_ptr<ICmdView>>;
    void createCmdViews(CmdViewVector&, d1::HMENU) const;

    using ResIdCmdMap = std::map<d1::WORD, d1::UINT>;
    static ResIdCmdMap defaults_;

    struct RegEntry;

    d1::UINT setDefaultCommand();
};


template <class T>
using Exec = void (T::*)(const WinUtil::WM_COMMAND_Msg&);

template <class T>
using Prep = bool (T::*)(const WinUtil::WM_COMMAND_Msg&);


class ICommand
{
    const int id_ = 0;
    ICommander* commander_ = nullptr;
    bool enabled_ = true;
    WinUtil::IExceptionHandler* exceptionHandler_ = nullptr; // may be zero, no ownership

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
};


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
