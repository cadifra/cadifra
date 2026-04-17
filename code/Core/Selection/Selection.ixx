/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core:Selection;

import std;


namespace Core
{

export class ElementSet;
export class IElement;
export class IView;


namespace Selection
{

export class Tracker
{
    IView* view_ = nullptr; // may be zero (dummy Tracker)
    bool selectionChanged_ = false;

public:
    Tracker(IView* v):
        view_{ v }
    {
    }

    Tracker(const Tracker&) = delete;
    Tracker& operator=(const Tracker&) = delete;

    ~Tracker(); // intentionally NOT virtual

    void changed() { selectionChanged_ = true; }
};


export class IRestorer
{
public:
    IRestorer() = default;

    virtual ~IRestorer() = default;

    IRestorer(const IRestorer&) = delete;
    IRestorer& operator=(const IRestorer&) = delete;

    virtual void restore(Tracker&, IView&) = 0;
};

export using IRestorerRef = std::shared_ptr<IRestorer>;


export class Hider;

export class VisibilityServer
{
public:
    VisibilityServer(IView& v):
        view_{ v }
    {
    }

    VisibilityServer(const VisibilityServer& rhs) = delete;
    VisibilityServer& operator=(const VisibilityServer& rhs) = delete;

    ~VisibilityServer();

    Hider hideSelection();
    // Makes the selection invisible in view_. If the returned Hider
    // is destructed, the selection is made visible again (if there are no
    // other selection hiders at this VisibilityServer)

    class Imp;

private:
    std::shared_ptr<Imp> imp_;
    IView& view_;
};

class VisibilityServer::Imp final
{
    friend class VisibilityServer;
    friend class Hider;

    unsigned long numOfServers_{};
    IView* view_{};
    unsigned long numOfHiders_{};

    void addServer(IView& v);
    void releaseServer();

    void addHider();
    void removeHider();
};


export class Hider // has value semantics
{
    using VS = VisibilityServer;

    friend class VS;

    std::shared_ptr<VS::Imp> serverImp_; // may be zero

public:
    Hider() = default;
    ~Hider(); // intentionally NOT virtual

    Hider(const Hider& rhs);
    Hider& operator=(const Hider& rhs);

private:
    Hider(const std::shared_ptr<VS::Imp>& server);
};

}

export auto makeStandardSelectionRestorer(
    const ElementSet& selection) -> Selection::IRestorerRef;


namespace ExtendSelection
{

export enum class Result {
    no = 0,
    possibly,
    yes
};


export class Param
{
public:
    class ICaller;

    Param(const ICaller& caller, const ElementSet& selection);
    ~Param();

    Param(const Param&) = delete;
    Param& operator=(const Param&) = delete;

    auto call(const IElement* target) -> Result;

    auto selection() const -> const ElementSet& { return selection_; }

private:
    struct CacheEntry;
    using Cache = std::multimap<const IElement*, CacheEntry>;

    bool findCacheEntry(const IElement* target, Result& res) const;

    void eraseOtherCacheEntries(const Cache::iterator except);

    Cache cache_;
    const ICaller& caller_;
    const ElementSet& selection_;
    const IElement* previous_ = nullptr;
};


class Param::ICaller
{
public:
    virtual Result makeCall(const IElement* target, Param& p) const = 0;

protected:
    ~ICaller() = default;
};

}

}
