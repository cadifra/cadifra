/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;


namespace Core
{

ISelectionObserver::~ISelectionObserver()
{
    if (itsDock)
    {
        itsDock->Detach(*this);
        itsDock = nullptr;
    }
}

}
