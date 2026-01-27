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

    d1::UINT getMsgId() const { return message_; }
    d1::WPARAM getWParam() const { return WParam_; }
    d1::LPARAM getLParam() const { return LParam_; }
    d1::LRESULT getLResult() const { return LResult_; }

    void setResult(d1::LRESULT result);
    // Sets the result that will be returned to the system, when
    // this message has been executed.
    // If this function is not called, GetLResult will return 0.
    // PRE: EnableOS has not been called
    //      SetDefProcCalled has not been called

    bool resultSet() const { return setResultCalled_; }
    // Returns true if SetResult was called.

    void enableOS();
    // Permits the OS to process the message (Default = disabled)
    // When this message has been executed, the executer will
    // call the corresponding DefProc for this message.
    // PRE: SetResult has not been called.
    //      SetDefProcCalled has not been called.

    bool OSisEnabled() const { return OSisEnabledFlag_; }
    // Returns true, if EnableOS has been called

    void defProcCalled(d1::LRESULT result);
    // This function is called by WinUtil::CallDefProcNow, when the
    // DefProc for this message has been called.
    // PRE: DefProcCalled has not been called.

    bool defProcCalled() const { return defProcCalled_; }
    // Returns true, if SetDefProcCalled has been called.

    class Wrapper;

private:
    bool OSisEnabledFlag_ = false;
    d1::UINT message_ = {};
    d1::WPARAM WParam_ = {};
    d1::LPARAM LParam_ = {};
    d1::LRESULT LResult_ = 0;
    bool setResultCalled_ = false;
    bool defProcCalled_ = false;
};



inline Message::Message(d1::UINT message, d1::WPARAM wParam, d1::LPARAM lParam):
    message_{ message },
    WParam_{ wParam },
    LParam_{ lParam }
{
}

inline void Message::setResult(d1::LRESULT result)
{
    D1_ASSERT(not OSisEnabledFlag_);
    D1_ASSERT(not defProcCalled_);
    LResult_ = result;
    setResultCalled_ = true;
}

inline void Message::enableOS()
{
    D1_ASSERT(not setResultCalled_);
    D1_ASSERT(not defProcCalled_);
    OSisEnabledFlag_ = true;
}

inline void Message::defProcCalled(d1::LRESULT result)
{
    D1_ASSERT(not defProcCalled_);
    LResult_ = result;
    defProcCalled_ = true;
}


export class Message::Wrapper
{
public:
    void enableOS() const
    {
        msg_.enableOS();
    }

    bool OSisEnabled() const
    {
        return msg_.OSisEnabled();
    }

    void setResult(d1::LRESULT result) const
    {
        msg_.setResult(result);
    }


    d1::WPARAM getWParam() const { return msg_.getWParam(); }
    d1::LPARAM getLParam() const { return msg_.getLParam(); }

    Message& getWinMsg() { return msg_; }

protected:
    Wrapper(Message& msg, d1::UINT check):
        msg_{ msg }
    {
        D1_ASSERT(msg.getMsgId() == check);
    }

private:
    Message& msg_;

    // uses compiler generated copy ctor and assignment operator
};


// Parent class for all message wrappers with a message id known
// at compile time (all message wrappers for predefined
// windows messages).

export template <d1::UINT MsgId>
class StaticWinMsgWrapper: public Message::Wrapper
{
public:
    static d1::UINT getMsgId() { return MsgId; }

protected:
    StaticWinMsgWrapper(Message& msg):
        Message::Wrapper{ msg, MsgId }
    {
    }
};

}
