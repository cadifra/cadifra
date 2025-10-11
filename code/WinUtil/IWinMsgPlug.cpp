/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

module WinUtil;


namespace WinUtil
{

void IWinMsgPlug::Process(WinMsg& msg)
{
    if (!itsExceptionHandler)
    {
        ProcessImp(msg);
        return;
    }

    D1_ASSERT(itsExceptionHandler);

    try
    {
        ProcessImp(msg);
    }
    catch (...)
    {
        itsExceptionHandler->HandleException();
        throw;
    }
}


}

