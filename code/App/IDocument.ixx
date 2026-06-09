/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module App.IDocument;

import App.NumberedWindow;
import App.LifeTimeSupervisor;
import App.IApplication;
import App.NewWindowDefaultsProvider;
import App.IWindow;

import d1.Observer;
import d1.wintypes;

import WinUtil.Window;
import WinUtil.Messages;

import std;


namespace App
{

export class IDocument:
    public IWindow::Creator,
    public LifeTimeSupervisor::Client
{
public:
    class IFactory;

    IDocument(IFactory&, LifeTimeSupervisor& lts);
    IDocument(const IDocument&) = delete;
    IDocument& operator=(const IDocument&) = delete;
    virtual ~IDocument();

    //-- IWindow::Creator

    int getWindowCount() const override;
    void forget(IWindow&) override;
    auto getApplication() const -> IApplication& override;
    void setLastActiveWindow(IWindow&) override;

    //-- ILifeTimeSupervisorClient

    void shutDown() override;

    //--

    void showWindow(const NewWindowDefaultsProvider*);

    void setName(std::wstring name);
    std::wstring getName() const { return name_; }

    virtual bool isEmbedded() const = 0;
    virtual bool isNew() const = 0;

    class INameChange;
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


class IDocument::IFactory
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


class IDocument::IFactory::Creator
{
public:
    virtual auto getApplication() -> IApplication& = 0;

protected:
    ~Creator() = default;
};


class IDocument::INameChange
{
public:
    virtual void documentNameChanged(std::wstring) = 0;

protected:
    ~INameChange() = default;
};

}

module : private;

import d1.Observer;


namespace App
{

namespace
{
using C = IDocument;
}


C::IDocument(IFactory& creator, LifeTimeSupervisor& lts):
    creator_{ creator },
    lifeTimeSupervisor_{ lts },
    windowNumberer_{ std::make_unique<INumberedWindow::Numberer>() }
{
}


C::~IDocument()
{
    if (not hasShutDown_)
        creator_.forget(*this);
}


void C::addWindow(IWindow& w)
{
    D1_ASSERT(windows_.end() == std::ranges::find(windows_, &w));

    windows_.push_back(&w);
    windowNumberer_->add(w);
    lastActiveWindow_ = &w;
}


void C::forget(IWindow& w)
{
    if (lastActiveWindow_ == &w)
        lastActiveWindow_ = nullptr;

    windowNumberer_->forget(w);

    auto n = std::erase(windows_, &w);
    D1_ASSERT(n);

    // The deletion of the last window should force the diagram to shut down
    // even if there are still external COM connections.
    // (The user sees the diagram closing, so we should do what he sees).
    if (windows_.empty())
        lifeTimeSupervisor_.forceShutDown();
}


void C::setLastActiveWindow(IWindow& w)
{
    auto i = std::ranges::find(windows_, &w);

    if (i != windows_.end())
        lastActiveWindow_ = &w;
}


int C::getWindowCount() const
{
    return static_cast<int>(windows_.size());
}


auto C::getLifeTimeSupervisor() -> LifeTimeSupervisor&
{
    return lifeTimeSupervisor_;
}


bool C::visitWindows(IWindowVisitor& v) const
{
    auto visit = [&](auto* w) { return not v.visit(*w); };

    for (auto* w : windows_ | std::views::filter(visit))
        return false;

    return true;
}


auto C::getApplication() const -> IApplication&
{
    return creator_.getApplication();
}


void C::shutDown()
{
    creator_.forget(*this);
    hasShutDown_ = true;
}


void C::showWindow(const NewWindowDefaultsProvider* dp)
{
    if (not windows_.size())
        createNewWindow(dp);

    D1_ASSERT(windows_.size());

    auto w = lastActiveWindow_;

    if (not w)
        w = windows_.front();
    D1_ASSERT(w);

    HWND wnd = w->getWindowHandle();

    if (::IsIconic(wnd))
    {
        D1_VERIFY(::ShowWindow(wnd, SW_RESTORE));
    }

    ::BringWindowToTop(wnd);
    ::SetForegroundWindow(wnd);
}


void C::setName(std::wstring name)
{
    if (name != name_)
    {
        name_ = name;
        notifyDocumentNameObservers();
    }
}


void C::notifyDocumentNameObservers()
{
    nameObservers_.notify(
        [this](INameChange* obs) {
            obs->documentNameChanged(name_);
        });
}


auto C::getNameChangeConnector() -> d1::Observer::C<INameChange>
{
    return nameObservers_.getConnector();
}


C::IFactory::IFactory(
    Creator& creator,
    LifeTimeSupervisor& s):

    creator_{ creator },
    lifeTimeSupervisor_{ s },
    app_{ creator.getApplication() }
{
}


auto C::IFactory::getApplication() const -> IApplication&
{
    return app_;
}


bool C::IFactory::askForUserClose()
{
    class V: public IVisitor
    {
        bool visit(IDocument& d) final { return d.askForUserClose(); }
    } v;

    return visitDocuments(v);
}


auto C::IFactory::getLifeTimeSupervisor() -> LifeTimeSupervisor&
{
    return lifeTimeSupervisor_;
}

}
