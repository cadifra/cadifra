/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

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
