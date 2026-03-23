/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core:View;


namespace Core
{

ISelectionObserver::~ISelectionObserver()
{
    if (dock_)
    {
        dock_->detach(*this);
        dock_ = nullptr;
    }
}

}
