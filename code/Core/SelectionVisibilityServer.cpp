/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module Core;


namespace Core
{

namespace
{
using C = SelectionVisibilityServerImp;
}


void C::AddServer(IView& v)
{
    if (!itsView)
        itsView = &v;
    D1_ASSERT(itsView == &v);
    ++itsNumOfServers;
}


void C::ReleaseServer()
{
    if (--itsNumOfServers == 0)
        itsView = nullptr;
}


void C::AddHider()
{
    if (itsNumOfHiders++ == 0)
        if (itsNumOfServers && itsView)
            itsView->SetSelectionVisibility(false);
}


void C::RemoveHider()
{
    if (--itsNumOfHiders == 0)
        if (itsNumOfServers)
            itsView->SetSelectionVisibility(true);
}


SelectionVisibilityServer::~SelectionVisibilityServer()
{
    if (itsImp)
        itsImp->ReleaseServer();
}


auto SelectionVisibilityServer::HideSelection() -> SelectionHider
{
    if (!itsImp)
    {
        itsImp = std::make_shared<SelectionVisibilityServerImp>();
        itsImp->AddServer(itsView);
    }
    return { itsImp };
}

}
