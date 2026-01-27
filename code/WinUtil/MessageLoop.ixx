/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil.MessageLoop;

import d1.wintypes;

import std;


namespace WinUtil
{

export class MessageFilter
{
public:
    static MessageFilter& instance();

    void activate(d1::HWND window);
    // The first call of this function registers the message
    // filter in the OS. The window handle is used as the owner
    // of message boxes. You may exchange the window handle
    // by calling activate again.

    void deactivate();
    // After calling activate, call deactivate before OleUninitialize.
    // It's allowed to call deactivate without a previous call to
    // activate.

    ~MessageFilter();

    class PostponeIncomingCalls;

private:
    class Impl;
    const std::unique_ptr<Impl> impl_;

    MessageFilter();

    void incPostponeIncomingCalls();
    void decPostponeIncomingCalls();

    MessageFilter(const MessageFilter&) = delete;
    MessageFilter& operator=(const MessageFilter&) = delete;
};


class MessageFilter::PostponeIncomingCalls
{
public:
    PostponeIncomingCalls() { instance().incPostponeIncomingCalls(); }
    ~PostponeIncomingCalls() { instance().decPostponeIncomingCalls(); }
};


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

    std::pair<bool, int> loopImpl();
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


/*
The class PrivateMessage allows the registration of an application local
window message.

With this class, you do not need to maintain a global header file that contains
all user defined messages which creates unwanted dependencies between unrelated
packages.

PrivateMessage uses message numbers in the range (WM_USER+0x200)...(WM_APP-1).

Note the difference to the operating system function RegisterWindowMessage,
which creates message numbers that are unique among all applications.
*/
export class PrivateMessage
{
public:
    static PrivateMessage& instance();

    virtual unsigned int getNumber() = 0;
    // returns a message number that is unique within the application

protected:
    ~PrivateMessage() = default;
};


export class SuspendQuit
// Temporarily removes in its ctor a WM_QUIT message from the
// message loop and reposts it in its dtor.
{
public:
    SuspendQuit();

    SuspendQuit(const SuspendQuit&) = delete;
    SuspendQuit& operator=(const SuspendQuit&) = delete;

    ~SuspendQuit(); // intentionally not virtual

private:
    bool removedQuitMessage_ = false;
    int exitCode_ = -1;
};

}
