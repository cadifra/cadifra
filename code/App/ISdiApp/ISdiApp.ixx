/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module App.ISdiApp;

import App.IApplication;

import d1.Observer;
import d1.types;
import d1.wintypes;

import WinUtil.IWindow;
import WinUtil.Messages;

import std;


namespace App
{

namespace Document
{
export class IFactory;
}


export class LifeTimeSupervisor
{
    class TreeInfo;

public:
    LifeTimeSupervisor();
    LifeTimeSupervisor(LifeTimeSupervisor& parent, const std::shared_ptr<TreeInfo>&);

    LifeTimeSupervisor(const LifeTimeSupervisor&) = delete;
    LifeTimeSupervisor& operator=(const LifeTimeSupervisor&) = delete;

    ~LifeTimeSupervisor(); // intentionally not virtual


    d1::uint32 lock();
    d1::uint32 unlock(bool lastUnlockShutsDown = true);

    d1::uint32 AddRef();  // increments the reference count
    d1::uint32 Release(); // decrements the reference count

    void forceShutDown();
    // Clears all locks of this LifeTimeSupervisor and its children.
    // Calls ShutDown of its children and its client.
    // The children and the client will be deleted after ShutDown as soon
    // as the reference count becomes zero.

    class Client;

    void setClient(std::unique_ptr<Client>);
    // SetClient may be called only once.

    class Guard;

    Guard createChild();
    // Creates a new LifeTimeSupervisor that is added as a child to
    // this LifeTimeSupervisor. You can access the new LifeTimeSupervisor
    // via Guard::get().

#ifdef _DEBUG
    void dump(int level = -1) const;
#endif

private:
    LifeTimeSupervisor* parent_ = nullptr;
    std::shared_ptr<TreeInfo> treeInfo_;
    // A tree of LifeTimeSupervisors shares a common TreeInfo. The root of the tree
    // creates the TreeInfo.

    std::unique_ptr<Client> client_;

    bool hasShutDown_ = false;

    d1::uint32 lockCount_ = 0;
    d1::uint32 refCount_ = 0;

    using Children = std::vector<std::unique_ptr<LifeTimeSupervisor>>;
    Children children_;

    class ShutDownPhase;

    void upShutDown();
    void shutDown();
    bool hasLocks() const;
    void removeLocks();
    void clientShutDown();
    bool deleteGarbage();
};


export class LifeTimeSupervisor::Client
{
public:
    Client() = default;
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    virtual ~Client() = 0;

    virtual void shutDown() {};
    // ShutDown will be called on completely constructed objects (before
    // the dtor is called).
};

// Read the description in LifeTimeSupervisor to know who is
// the owner of a Client.

inline LifeTimeSupervisor::Client::~Client() = default;


class LifeTimeSupervisor::Guard
{
    LifeTimeSupervisor* LTS_ = nullptr;
    friend LifeTimeSupervisor;
    Guard(LifeTimeSupervisor& lts):
        LTS_{ &lts } {}
    Guard& operator=(const Guard&) = delete;

public:
    Guard(Guard&& g) noexcept;
    ~Guard();
    void dismiss() { LTS_ = 0; }
    Client* setClient(std::unique_ptr<Client> c);
    LifeTimeSupervisor* get() const { return LTS_; }
};


/*
How to use:
===========

using LTS = LifeTimeSupervisor;

class X: public LTS::Client
{
    LTS& Lts_; // only a reference!
    ...
    void createZ();
};

X::X(LTS& lts): lts_(lts) {}

X::createZ()
{
    auto guard = LTS::Guard{ Lts_.createChild() };
    guard.setClient(std::make_unique<Z>(*guard.get())); // Z may add grandchildren
    guard.dismiss();
}

// Never write a class, that adds elf_ (in the ctor) as a client to an LTS.
// Only the creator of an object should add it to an LTS.
// So an LTS object is owned only by its parent.
// The very first LTS object is on the stack:

void main()
{
    LTS lts;
    lts.setClient(std::make_unique<X>(lts));  // X uses lts to add children
}

*/



export class INumberedWindow
{
public:
    class Numberer;

    virtual void setWindowNumber(int number) = 0;

protected:
    ~INumberedWindow() = default;
};


export class NewWindowDefaultsProvider
{
public:
    NewWindowDefaultsProvider() = default;
    virtual ~NewWindowDefaultsProvider() = 0;

    NewWindowDefaultsProvider(const NewWindowDefaultsProvider&) = delete;
    NewWindowDefaultsProvider& operator=(const NewWindowDefaultsProvider&) = delete;
};


inline NewWindowDefaultsProvider::~NewWindowDefaultsProvider() = default;


namespace Document
{

export class IWindow:
    public WinUtil::Window,
    public INumberedWindow,
    public LifeTimeSupervisor::Client
{
    using This = IWindow;

public:
    class Creator;

    IWindow(Creator&, LifeTimeSupervisor& lts);
    IWindow(const IWindow&) = delete;
    IWindow& operator=(const IWindow&) = delete;

    bool askForUserClose();

    auto getApplication() const -> IApplication&;

    bool isActive() const { return itIsActive; }

    auto getLifeTimeSupervisor() -> LifeTimeSupervisor&;

    //-- ILifeTimeSupervisorClient

    void shutDown() override;

    //--

private:
    Creator& creator_;
    LifeTimeSupervisor& lifeTimeSupervisor_;
    bool itIsActive = true;

    virtual void changeActiveState(bool isActive) = 0;

    WinUtil::ProcRegistrar registrar_;
    void onNCActivate(WinUtil::WM_NCACTIVATE_Msg);
    void onDestroy(WinUtil::WM_DESTROY_Msg);
    void onClose(WinUtil::WM_CLOSE_Msg);
    void onQueryEndSession(WinUtil::WM_QUERYENDSESSION_Msg);
};


class IWindow::Creator
{
public:
    virtual int getWindowCount() const = 0;
    virtual void forget(IWindow&) = 0;

    virtual auto getApplication() const -> IApplication& = 0;

    virtual void changeActiveState(bool isActive) = 0;

    virtual bool askForUserClose() = 0;

    virtual void setLastActiveWindow(IWindow&) = 0;

protected:
    ~Creator() = default;
};


export class INameChange
{
public:
    virtual void documentNameChanged(std::wstring) = 0;

protected:
    ~INameChange() = default;
};


export class IWindowVisitor
{
public:
    virtual bool visit(IWindow&) = 0;

protected:
    ~IWindowVisitor() = default;
};


export class IDocument:
    public IWindow::Creator,
    public LifeTimeSupervisor::Client
{
public:
    IDocument(IFactory&, LifeTimeSupervisor& lts);
    IDocument(const IDocument&) = delete;
    IDocument& operator=(const IDocument&) = delete;
    virtual ~IDocument();

    //-- Document::IWindow::Creator

    int getWindowCount() const override;
    void forget(Document::IWindow&) override;
    auto getApplication() const -> IApplication& override;
    void setLastActiveWindow(Document::IWindow&) override;

    //-- ILifeTimeSupervisorClient

    void shutDown() override;

    //--

    void showWindow(const NewWindowDefaultsProvider*);

    void setName(std::wstring name);
    std::wstring getName() const { return name_; }

    virtual bool isEmbedded() const = 0;
    virtual bool isNew() const = 0;

    auto getNameChangeConnector() -> d1::Observer::C<INameChange>;

    virtual d1::CLSID getCLSID() const = 0;

    void addWindow(IWindow&); // does not take ownership

    auto getLifeTimeSupervisor() -> LifeTimeSupervisor&;

    auto getLastActiveWindow() const -> IWindow*
    {
        return lastActiveWindow_;
    }

    bool visitWindows(IWindowVisitor& v) const;
    // Calls v->visit(w) for every IDocumentWindow w as long as v->Visit returns true.
    // Returns true if all Visit calls returned true.

private:
    IFactory& creator_;
    LifeTimeSupervisor& lifeTimeSupervisor_;
    std::wstring name_;
    bool hasShutDown_ = false;

    const std::unique_ptr<INumberedWindow::Numberer> windowNumberer_;


    using ItsWindows = std::vector<IWindow*>;
    ItsWindows windows_; // no ownership

    IWindow* lastActiveWindow_ = nullptr; // may be 0

    d1::Observer::L<INameChange> nameObservers_;

    void notifyDocumentNameObservers();

    virtual void createNewWindow(const NewWindowDefaultsProvider*) = 0;
};


export class IVisitor
{
public:
    virtual bool visit(IDocument&) = 0;

protected:
    ~IVisitor() = default;
};


export class IFactory
{
public:
    class Creator;

    IFactory(Creator&, LifeTimeSupervisor&);
    IFactory(const IFactory&) = delete;
    IFactory& operator=(const IFactory&) = delete;
    virtual ~IFactory() = default;


    auto getApplication() const -> IApplication&;

    bool askForUserClose();

    virtual bool visitDocuments(IVisitor& v) const = 0;
    // Calls v->visit(d) for every IDocument d as long as v->Visit returns true.
    // Returns true if all Visit calls returned true.

    virtual void forget(IDocument&) = 0;

    virtual void shutDown() {}

    auto getLifeTimeSupervisor() -> LifeTimeSupervisor&;

private:
    Creator& creator_;
    LifeTimeSupervisor& lifeTimeSupervisor_;
    IApplication& app_;
};


export class IFactory::Creator
{
public:
    virtual auto getApplication() -> IApplication& = 0;

protected:
    ~Creator() = default;
};

}

export using IDocument = Document::IDocument;


// A Numberer allocates numbers to the registered INumberedWindow
// objects.
// It calls the member function SetWindowNumber of every registered
// INumberedWindow if a new INumberedWindow is registered or an old one
// is unregistered.
//
// The allocated numbers start with the number 1 if more than one
// INumberedWindow is registered; it starts with 0 if only one
// INumberedWindow exists.

class INumberedWindow::Numberer
{
public:
    Numberer() {}
    Numberer(const Numberer&) = delete;
    Numberer& operator=(const Numberer&) = delete;

    void add(INumberedWindow& window);
    // register a new INumberedWindow

    void forget(INumberedWindow& window);
    // unregister a INumberedWindow

private:
    using ItsWindowsType = std::vector<INumberedWindow*>; // ref only ptrs
    ItsWindowsType windows_;
    void allocateNumbers();
};


export class CommandLine
{
public:
    CommandLine(const std::wstring& cmdLine);
    // Sends cmdLine to the process that is the master if there is
    // another process from the same module (exe-file). If there is
    // no master process, the calling process becomes master and cmdLine
    // is stored for later execution.
    // If this process becomes master, call set (the master access
    // right is blocked until you call set). Otherwise exit this process.

    CommandLine(const CommandLine&) = delete;
    CommandLine& operator=(const CommandLine&) = delete;

    ~CommandLine();


    enum class Status
    {
        Failed,
        Slave,
        Master
    };

    Status getStatus() const;

    class IReceiver;

    void set(LifeTimeSupervisor&, IReceiver& r);
    // r will get the cmdLine value from the constructor call and the
    // cmdLine values from other processes.

    void stopReceive();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};


class CommandLine::IReceiver
{
public:
    virtual ~IReceiver() = default;
    virtual void interpretCommandLine(const std::wstring&) = 0;
};


class OleInitializer
{
public:
    OleInitializer();
    OleInitializer(const OleInitializer&) = delete;
    OleInitializer& operator=(const OleInitializer&) = delete;

    ~OleInitializer(); // intentionally not virtual

    bool isInitialized() const { return itIsInitialized; }

private:
    bool itIsInitialized;
};


export class ISdiApp:
    public IApplication,
    private CommandLine::IReceiver
{
public:
    ISdiApp() = default;
    ISdiApp(const ISdiApp&) = delete;
    ISdiApp& operator=(const ISdiApp&) = delete;

    virtual ~ISdiApp();

    int WinMain(
        CommandLine&,
        WinUtil::HINSTANCE hInstance,     // handle to current instance
        WinUtil::HINSTANCE hPrevInstance, // handle to previous instance
        int nCmdShow);                    // show state of window


    virtual int init() = 0;
    // Makes any initializations and returns 0 if ok

    virtual auto createDocumentFactory(LifeTimeSupervisor&)
        -> Document::IFactory& = 0;


    //-- IApplication

    auto getMessageLoop() -> WinUtil::MessageLoop& override
    {
        return messageLoop_;
    }

    WinUtil::HINSTANCE getInstanceHandle() const override { return hInstance_; }

    //--


private:
    OleInitializer oleInitializer_;
    // oleInitializer_ has to be the first member variable!

    WinUtil::HINSTANCE hInstance_{};
    WinUtil::MessageLoop messageLoop_;
    class Quit;
};

}
