/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil:Timer;

namespace WinUtil
{

export class ITimerClient
{
public:
    virtual void TimerElapsed() = 0;

protected:
    ~ITimerClient() = default;
};


export class Timer
{
    class Impl;
    static Impl& Instance();

    bool itIsRunning = false;
    unsigned int itsId = 0;
    ITimerClient& itsClient;

public:
    Timer(ITimerClient&);
    // creates a timer without starting it

    Timer(ITimerClient&, unsigned int milliseconds);
    // creates a timer and calls Start

    ~Timer();
    // calls Stop
    // intentionally not virtual

    void Start(unsigned int milliseconds);
    // calls Stop and starts the timer

    void Stop();
    // stops the timer if it's running

    bool IsRunning() const { return itIsRunning; }
};


// Timer uses windows messages. The ITimerClient::TimerElapsed function
// is only called while the windows message loop is working.

}
