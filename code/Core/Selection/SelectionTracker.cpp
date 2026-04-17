/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;
import :Selection;

import :View;


namespace Core
{

Selection::Tracker::~Tracker()
{
    if (view_ and selectionChanged_)
        view_->notifySelectionObservers();
}

}
