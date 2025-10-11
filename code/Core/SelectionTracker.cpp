/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;


namespace Core
{

SelectionTracker::~SelectionTracker()
{
    if (itsView && itsSelectionChanged)
        itsView->NotifySelectionObservers();
}

}
