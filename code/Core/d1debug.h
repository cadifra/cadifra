#pragma once
/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

import WinUtil.Debug;

import Core.ObjectID;

import Core;


#ifdef _DEBUG
#define D1_DEBUG_DEFINE_FLAG(category, flag) \
namespace { \
  const bool d1DebugEnabled = \
    (WinUtil::DebugEnv::inst().getInt(#category, #flag) != 0); \
}
#else
#define D1_DEBUG_DEFINE_FLAG(category, flag)  
#endif


#ifdef _DEBUG
#define D1_DEBUG_DEFINE_MODULE(module) \
namespace { \
  const char* d1module = #module; \
}
#else
#define D1_DEBUG_DEFINE_MODULE(module)  
#endif


#ifdef _DEBUG
  #define D1_DEBUG_PREFIX(line)  \
    if (d1DebugEnabled) \
      WinUtil::dout  << d1module << " L" << line \
                     << "(" << /*e.refCount()*/0 << ")" \
                     << " #" << this->getID().val()

  #define D1_DEBUG_PREFIX_L  D1_DEBUG_PREFIX(__LINE__)
#endif


#ifdef _DEBUG
  #define D1_DEBUG_STMT(statement) { \
    D1_DEBUG_PREFIX_L << ": STMT  " #statement << std::endl; \
    statement;  }
#else
  #define D1_DEBUG_STMT(statement) { statement; }
#endif


#ifdef _DEBUG
  #define D1_DEBUG_DEFI(definition) \
    D1_DEBUG_PREFIX_L << ": DEFI  " #definition << std::endl; \
    definition; 
#else
  #define D1_DEBUG_DEFI(definition)  definition;
#endif


#ifdef _DEBUG
  #define D1_DEBUG_WATCH(expr) \
    D1_DEBUG_PREFIX_L << ": WATCH  " #expr":" << (expr) << std::endl;
#else
  #define D1_DEBUG_WATCH(expr)
#endif



#ifdef _DEBUG
  #define D1_DEBUG_WATCH_ID(ptr) \
    D1_DEBUG_PREFIX_L << ": WATCH  " #ptr ": #" \
                      << ((ptr) ? (ptr)->getID().val() : 0) << std::endl;
#else
  #define D1_DEBUG_WATCH_ID(ptr)
#endif


#ifdef _DEBUG

  namespace {
    class d1Exit  {
      const bool d1DebugEnabled; const char* d1module; long line; 
      Core::ObjectID id; Core::Env& e; const char* mf;  
    public:
      d1Exit(bool de, const char* mod, long line,
             Core::ObjectID id, Core::Env& e, const char* mf)
      : d1DebugEnabled(de), d1module(mod), line(line), id(id), e(e), mf(mf)  { }
      Core::ObjectID getID() const { return id; }
      ~d1Exit() {  D1_DEBUG_PREFIX(line) << ": EXIT " << mf << std::endl;  }
    };
  }

  #define D1_DEBUG_MEMFUN(name) \
    D1_DEBUG_PREFIX(__LINE__) << ": @@@ " << #name << std::endl; \
    d1Exit tmp_core_d1debug_d1exit( \
      d1DebugEnabled, d1module, __LINE__, this->getID(), e, #name);

#else
  #define D1_DEBUG_MEMFUN(name)
#endif

