#pragma once
/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

#ifdef _DEBUG

  #define D1_TRACE_DEFINE_FLAG(category, flag) \
  namespace \
  { \
    static bool debugOut = WinUtil::DebugEnv::Inst().GetInt( \
      #category, #flag) != 0; \
  }

  #define D1_TRACE(s) if(debugOut) { WinUtil::dout << s << std::endl; }

#else

  #define D1_TRACE_DEFINE_FLAG(category, flag)
  #define D1_TRACE(s)

#endif
