/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

#include "WinUtil/d1Trace.h"

export module App.LifeTimeSupervisor;

import d1.types;

import std;


namespace App
{

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

}


module : private;

import WinUtil.Debug;

import std;


D1_TRACE_DEFINE_FLAG(LifeTime, App::LifeTimeSupervisor)


namespace App
{

using C = LifeTimeSupervisor;


C::Guard::Guard(Guard&& g) noexcept:
    LTS_{ std::exchange(g.LTS_, nullptr) }
{
}


C::Guard::~Guard()
{
    if (LTS_)
        LTS_->forceShutDown();
}


auto C::Guard::setClient(std::unique_ptr<Client> c) -> Client*
{
    auto res = c.get();
    if (not LTS_ or not c)
        return 0;
    LTS_->setClient(std::move(c));
    return res;
}


class C::TreeInfo
{
    d1::uint32 shutDownPhaseCount_ = 0;
    LifeTimeSupervisor& root_;

public:
    TreeInfo(LifeTimeSupervisor& root):
        root_{ root }
    {
    }

    void incrementShutDownPhase()
    {
        ++shutDownPhaseCount_;
    }

    void decrementShutDownPhase()
    {
        --shutDownPhaseCount_;
        deleteGarbage();
    }

    void deleteGarbage()
    {
        if (shutDownPhaseCount_)
            return;
        root_.deleteGarbage();
    }
};


class C::ShutDownPhase
{
    std::shared_ptr<TreeInfo> treeInfo_;

public:
    ShutDownPhase(const std::shared_ptr<TreeInfo>& ti):
        treeInfo_{ ti }
    {
        treeInfo_->incrementShutDownPhase();
    }

    ~ShutDownPhase()
    {
        treeInfo_->decrementShutDownPhase();
    }
};


C::LifeTimeSupervisor():
    treeInfo_{ std::make_shared<TreeInfo>(*this) }
{
    D1_TRACE("LS@" << this << "->LS()");
}


C::LifeTimeSupervisor(
    LifeTimeSupervisor& parent, const std::shared_ptr<TreeInfo>& t):

    parent_{ &parent },
    treeInfo_{ t }
{
    D1_TRACE("LS@" << this << "->LS(" << &parent << ")");
}


C::~LifeTimeSupervisor()
{
    D1_TRACE("LS@" << this << "->~LS()");
    forceShutDown();
}


auto C::createChild() -> Guard
{
    auto lts = std::make_unique<LifeTimeSupervisor>(*this, treeInfo_);
    auto guard = Guard{ *lts };
    children_.insert(begin(children_), std::move(lts));
    return guard;
}


void C::removeLocks()
{
    for (const auto& i : children_)
        i->removeLocks();
    lockCount_ = 0;
}


bool C::deleteGarbage()
{
    for (auto it = begin(children_); it != end(children_);) // no ++it
    {
        if ((*it)->deleteGarbage())
            it = children_.erase(it);
        else
            ++it;
    }

    if (not children_.empty() or refCount_ or not hasShutDown_)
        return false;

    client_.reset();
    return true;
}


d1::uint32 C::AddRef()
{
    D1_TRACE("LS@" << this << "->AddRef()  = " << refCount_ + 1);
    return ++refCount_;
}


d1::uint32 C::Release()
{
    D1_TRACE("LS@" << this << "->Release() = " << refCount_ - 1);

    if (not refCount_)
        return 0;

    auto res = --refCount_;

    if (not refCount_)
        treeInfo_->deleteGarbage();

    return res;
}


d1::uint32 C::lock()
{
    D1_TRACE("LS@" << this << "->lock() = " << lockCount_ + 1);
    return ++lockCount_;
}


d1::uint32 C::unlock(bool fLastUnlockShutsDown)
{
    D1_TRACE("LS@" << this << "->unlock("
                   << fLastUnlockShutsDown << ") = " << lockCount_ - 1);

    if (not lockCount_)
        return 0;

    auto res = --lockCount_;

    if (not lockCount_ and fLastUnlockShutsDown)
        upShutDown();

    return res;
}


void C::upShutDown()
{
    auto sp = ShutDownPhase{ treeInfo_ };

    for (auto* i = this; i; i = i->parent_)
        i->shutDown();
}


void C::shutDown()
{
    auto sp = ShutDownPhase{ treeInfo_ };

    if (hasLocks())
        return;
    clientShutDown();
}


bool C::hasLocks() const
{
    if (lockCount_)
        return true;
    for (const auto& i : children_)
        if (i->hasLocks())
            return true;
    return false;
}


void C::clientShutDown()
{
    if (hasShutDown_)
        return;
    hasShutDown_ = true;

    for (const auto& i : children_)
        i->clientShutDown();

    if (client_)
    {
        D1_TRACE("LS@" << this << " calls "
                       << typeid(*client_).name() << "@" << client_.get()
                       << "->shutDown()");
        client_->shutDown();
    }
}


void C::forceShutDown()
{
    if (hasShutDown_)
        return;
    D1_TRACE("LS@" << this << "->forceShutDown()");
    removeLocks();
    upShutDown();
}


void C::setClient(std::unique_ptr<Client> client)
{
    D1_TRACE("LS@" << this << "->setClient("
                   << typeid(*client).name() << "@" << &client << ")");

    D1_ASSERT(not client_.get());
    D1_ASSERT(client.get());

    client_ = std::move(client);
}


#ifdef _DEBUG
void C::dump(int level) const
{
    if (level < 0)
    {
        const auto* n = this;
        while (n->parent_)
            n = n->parent_;
        return n->dump(0);
    }

    for (int i = 0; i < level; ++i)
        WinUtil::dout << "  ";

    WinUtil::dout << "LS@" << this << " hasShutDown_=" << hasShutDown_;

    if (client_)
        WinUtil::dout << " client=\"" << typeid(*client_).name() << '"';

    WinUtil::dout << " locks=" << lockCount_ << " refs=" << refCount_;

    WinUtil::dout << std::endl;

    for (const auto& c : children_)
        c->dump(level + 1);
}
#endif

}
