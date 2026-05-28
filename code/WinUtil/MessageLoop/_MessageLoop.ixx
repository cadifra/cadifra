/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil.MessageLoop;

import d1.wintypes;

import std;


namespace WinUtil
{

using std::vector;

export class MessageLoop
{
public:
    class IPreProc;
    class IOneTimePostProc;
    class IIdleProc;

    MessageLoop();

    MessageLoop(const MessageLoop&) = delete;
    MessageLoop& operator=(const MessageLoop&) = delete;

    ~MessageLoop(); // intentionally not virtual

    int doLoop();
    // Stays in loop that processes messages as long as you do not
    // call ExitLoop or post a WM_QUIT message.

    void exitLoop();

    void processMessages();
    // Processes all messages in the message queue and returns.
    // Functions that consume a lot of processing time should repeatedly
    // call this function if they do not want to block the message
    // processing.
    // Example:
    // The "print abort" function is repeatedly called from printing functions
    // like "EndDoc". The "print abort" function calls "ProcessMessages"
    // which allows the user to press the cancel button of the
    // "print progress dialog".

private:
    vector<IPreProc*> preProc_;
    vector<IOneTimePostProc*> oneTimePostProc_;
    vector<IIdleProc*> idleProc_;

    vector<IPreProc*> waitForInsertPreProc_;
    vector<IOneTimePostProc*> waitForInsertOneTimePostProc_;
    vector<IIdleProc*> waitForInsertIdleProc_;

    void add(IPreProc&);
    void add(IOneTimePostProc&);
    void add(IIdleProc&);

    void forget(IPreProc&);
    void forget(IOneTimePostProc&);
    void forget(IIdleProc&);

    bool callPreProc(d1::MSG&);
    void callOneTimePostProc();
    void callIdleProc();

    auto loopImpl() -> std::pair<bool, int>;
};


class MessageLoop::IPreProc
{
    MessageLoop* messageLoop_ = nullptr;

public:
    IPreProc();
    void set(MessageLoop&);
    void clear();
    virtual bool preProcess(d1::MSG&) = 0 { return true; }
    // Return true to stop processing of the message

protected:
    ~IPreProc() { clear(); }
};


class MessageLoop::IOneTimePostProc
{
    MessageLoop* messageLoop_ = nullptr;

public:
    IOneTimePostProc();
    void set(MessageLoop&);
    void clear();
    virtual void postProcess() = 0
    { /*intentionally*/
    }

protected:
    ~IOneTimePostProc() { clear(); }
};


class MessageLoop::IIdleProc
{
    MessageLoop* messageLoop_ = nullptr;

public:
    IIdleProc();
    void set(MessageLoop&);
    void clear();
    virtual void idleProcess() = 0
    { /*intentionally*/
    }

protected:
    ~IIdleProc() { clear(); }
};

}
