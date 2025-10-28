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
using C = Selection::VisibilityServer;
}


void C::Imp::AddServer(IView& v)
{
    if (!itsView)
        itsView = &v;
    D1_ASSERT(itsView == &v);
    ++itsNumOfServers;
}


void C::Imp::ReleaseServer()
{
    if (--itsNumOfServers == 0)
        itsView = nullptr;
}


void C::Imp::AddHider()
{
    if (itsNumOfHiders++ == 0)
        if (itsNumOfServers && itsView)
            itsView->SetSelectionVisibility(false);
}


void C::Imp::RemoveHider()
{
    if (--itsNumOfHiders == 0)
        if (itsNumOfServers)
            itsView->SetSelectionVisibility(true);
}


C::~VisibilityServer()
{
    if (itsImp)
        itsImp->ReleaseServer();
}


auto C::HideSelection() -> Hider
{
    if (!itsImp)
    {
        itsImp = std::make_shared<VisibilityServer::Imp>();
        itsImp->AddServer(itsView);
    }
    return { itsImp };
}

}
