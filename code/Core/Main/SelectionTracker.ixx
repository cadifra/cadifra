/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:SelectionTracker;

import :IView;


namespace Core
{

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

    ~Tracker()
    {
        if (view_ and selectionChanged_)
            view_->notifySelectionObservers();
    }

    void changed() { selectionChanged_ = true; }
};

}

}
