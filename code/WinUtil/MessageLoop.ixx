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
    static MessageFilter& Instance();

    void Register(d1::HWND window);
    // The first call of this function registers the message
    // filter in the OS. The window handle is used as the owner
    // of message boxes. You may exchange the window handle
    // by calling Register again.

    void Unregister();
    // After calling Register, call Unregister before OleUninitialize.
    // It's allowed to call Unregister without a previous call to
    // Register.

    ~MessageFilter();

    class PostponeIncomingCalls;

private:
    class Impl;
    const std::unique_ptr<Impl> itsImpl;

    MessageFilter();

    void IncPostponeIncomingCalls();
    void DecPostponeIncomingCalls();

    MessageFilter(const MessageFilter&) = delete;
    MessageFilter& operator=(const MessageFilter&) = delete;
};


class MessageFilter::PostponeIncomingCalls
{
public:
    PostponeIncomingCalls() { Instance().IncPostponeIncomingCalls(); }
    ~PostponeIncomingCalls() { Instance().DecPostponeIncomingCalls(); }
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

    int DoLoop();
    // Stays in loop that processes messages as long as you do not
    // call ExitLoop or post a WM_QUIT message.

    void ExitLoop();

    void ProcessMessages();
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
    vector<IPreProc*> itsPreProc;
    vector<IOneTimePostProc*> itsOneTimePostProc;
    vector<IIdleProc*> itsIdleProc;

    vector<IPreProc*> itsWaitForInsertPreProc;
    vector<IOneTimePostProc*> itsWaitForInsertOneTimePostProc;
    vector<IIdleProc*> itsWaitForInsertIdleProc;

    void Register(IPreProc&);
    void Register(IOneTimePostProc&);
    void Register(IIdleProc&);

    void Unregister(IPreProc&);
    void Unregister(IOneTimePostProc&);
    void Unregister(IIdleProc&);

    bool CallPreProc(d1::MSG&);
    void CallOneTimePostProc();
    void CallIdleProc();

    std::pair<bool, int> LoopImpl();
};


class MessageLoop::IPreProc
{
    MessageLoop* itsMessageLoop = nullptr;

public:
    IPreProc();
    void Register(MessageLoop&);
    void Unregister();
    virtual bool PreProcess(d1::MSG&) = 0 { return true; }
    // Return true to stop processing of the message

protected:
    ~IPreProc() { Unregister(); }
};


class MessageLoop::IOneTimePostProc
{
    MessageLoop* itsMessageLoop = nullptr;

public:
    IOneTimePostProc();
    void Register(MessageLoop&);
    void Unregister();
    virtual void PostProcess() = 0
    { /*intentionally*/
    }

protected:
    ~IOneTimePostProc() { Unregister(); }
};


class MessageLoop::IIdleProc
{
    MessageLoop* itsMessageLoop = nullptr;

public:
    IIdleProc();
    void Register(MessageLoop&);
    void Unregister();
    virtual void IdleProcess() = 0
    { /*intentionally*/
    }

protected:
    ~IIdleProc() { Unregister(); }
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
    static PrivateMessage& Instance();

    virtual unsigned int Register() = 0;
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
    bool itRemovedQuitMessage = false;
    int itsExitCode = -1;
};

}
