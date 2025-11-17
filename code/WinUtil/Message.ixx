/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module WinUtil.Message;

import d1.wintypes;


namespace WinUtil
{

export class Message
{
public:
    Message(d1::UINT message, d1::WPARAM wParam, d1::LPARAM lParam);

    Message(const Message&) = delete;
    Message& operator=(const Message&) = delete;

    d1::UINT GetMsgId() const { return itsMessage; }
    d1::WPARAM GetWParam() const { return itsWParam; }
    d1::LPARAM GetLParam() const { return itsLParam; }
    d1::LRESULT GetLResult() const { return itsLResult; }

    void SetResult(d1::LRESULT result);
    // Sets the result that will be returned to the system, when
    // this message has been executed.
    // If this function is not called, GetLResult will return 0.
    // PRE: EnableOS has not been called
    //      SetDefProcCalled has not been called

    bool ResultSet() const { return itsSetResultCalled; }
    // Returns true if SetResult was called.

    void EnableOS();
    // Permits the OS to process the message (Default = disabled)
    // When this message has been executed, the executer will
    // call the corresponding DefProc for this message.
    // PRE: SetResult has not been called.
    //      SetDefProcCalled has not been called.

    bool OSisEnabled() const { return itsOSisEnabledFlag; }
    // Returns true, if EnableOS has been called

    void DefProcCalled(d1::LRESULT result);
    // This function is called by WinUtil::CallDefProcNow, when the
    // DefProc for this message has been called.
    // PRE: DefProcCalled has not been called.

    bool DefProcCalled() const { return itsDefProcCalled; }
    // Returns true, if SetDefProcCalled has been called.

    class Wrapper;

private:
    bool itsOSisEnabledFlag = false;
    d1::UINT itsMessage = {};
    d1::WPARAM itsWParam = {};
    d1::LPARAM itsLParam = {};
    d1::LRESULT itsLResult = 0;
    bool itsSetResultCalled = false;
    bool itsDefProcCalled = false;
};



inline Message::Message(d1::UINT message, d1::WPARAM wParam, d1::LPARAM lParam):
    itsMessage{ message },
    itsWParam{ wParam },
    itsLParam{ lParam }
{
}

inline void Message::SetResult(d1::LRESULT result)
{
    D1_ASSERT(!itsOSisEnabledFlag);
    D1_ASSERT(!itsDefProcCalled);
    itsLResult = result;
    itsSetResultCalled = true;
}

inline void Message::EnableOS()
{
    D1_ASSERT(!itsSetResultCalled);
    D1_ASSERT(!itsDefProcCalled);
    itsOSisEnabledFlag = true;
}

inline void Message::DefProcCalled(d1::LRESULT result)
{
    D1_ASSERT(!itsDefProcCalled);
    itsLResult = result;
    itsDefProcCalled = true;
}


export class Message::Wrapper
{
public:
    void EnableOS() const
    {
        itsMsg.EnableOS();
    }

    bool OSisEnabled() const
    {
        return itsMsg.OSisEnabled();
    }

    void SetResult(d1::LRESULT result) const
    {
        itsMsg.SetResult(result);
    }


    d1::WPARAM GetWParam() const { return itsMsg.GetWParam(); }
    d1::LPARAM GetLParam() const { return itsMsg.GetLParam(); }

    Message& GetWinMsg() { return itsMsg; }

protected:
    Wrapper(Message& msg, d1::UINT check):
        itsMsg{ msg }
    {
        D1_ASSERT(msg.GetMsgId() == check);
    }

private:
    Message& itsMsg;

    // uses compiler generated copy ctor and assignment operator
};


// Parent class for all message wrappers with a message id known
// at compile time (all message wrappers for predefined
// windows messages).

export template <d1::UINT MsgId>
class StaticWinMsgWrapper: public Message::Wrapper
{
public:
    static d1::UINT GetMsgId() { return MsgId; }

protected:
    StaticWinMsgWrapper(Message& msg):
        Message::Wrapper{ msg, MsgId }
    {
    }
};

}
