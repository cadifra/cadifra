#pragma once
/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */


#ifdef _DEBUG

import WinUtil.Debug;


namespace WinUtil
{

class DebugEntryExitReporter
{
    const char* itsText;
    bool itsEnabled;

public:
    DebugEntryExitReporter(bool enabled, const char* text):
        itsText{ text }, itsEnabled{ enabled }
    {
        if (itsEnabled)
            dout << itsText << " started." << std::endl;
    }
    ~DebugEntryExitReporter()
    {
        if (itsEnabled)
            dout << itsText << " finished." << std::endl;
    }
};


}


#define D1_DEBUG_REPORT_ENTRY_EXIT(condition, text) \
    WinUtil::DebugEntryExitReporter __d1_debug_entry_exit_reporter(condition, text);

#else // #ifdef _DEBUG


#define D1_DEBUG_REPORT_ENTRY_EXIT(condition, text)


#endif // #ifdef _DEBUG
