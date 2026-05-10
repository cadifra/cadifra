/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module App.Document;

import App.NumberedWindow;
import App.LifeTimeSupervisor;
import App.IApplication;
import App.NewWindowDefaultsProvider;

import d1.Observer;
import d1.wintypes;

import WinUtil.IWindow;
import WinUtil.Messages;


namespace App
{

namespace Document
{

export class IFactory;

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

}
