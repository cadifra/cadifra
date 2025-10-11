#pragma once
/*
 *     Copyright (c) 1998-2023 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

#define D1_WINMESSAGE_WRAPPER_SUBCLASS_BEGIN(ClassName, MsgId, Superclass)\
  class ClassName:\
    public Superclass<MsgId>\
  {\
  public:\
    ClassName(WinMsg& msg):\
      Superclass<MsgId>(msg) {}\

#define D1_WINMESSAGE_WRAPPER_SUBCLASS_END\
  };


#define D1_WINMESSAGE_WRAPPER_SUBCLASS_NO_PARAM(ClassName, MsgId, Superclass)\
  D1_WINMESSAGE_WRAPPER_SUBCLASS_BEGIN(ClassName, MsgId, Superclass)\
  D1_WINMESSAGE_WRAPPER_SUBCLASS_END




#define D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(ClassName, MsgId)\
  D1_WINMESSAGE_WRAPPER_SUBCLASS_BEGIN(ClassName, MsgId, StaticWinMsgWrapper)

#define D1_WINMESSAGE_WRAPPER_CLASS_END\
  D1_WINMESSAGE_WRAPPER_SUBCLASS_END


#define D1_WINMESSAGE_WRAPPER_CLASS_NO_PARAM(ClassName, MsgId)\
  D1_WINMESSAGE_WRAPPER_CLASS_BEGIN(ClassName, MsgId)\
  D1_WINMESSAGE_WRAPPER_CLASS_END



