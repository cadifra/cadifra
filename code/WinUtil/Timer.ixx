/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil.Timer;

namespace WinUtil
{

export class Timer
{
public:
    class Client;

    Timer(Client&);
    // creates a timer without starting it

    Timer(Client&, unsigned int milliseconds);
    // creates a timer and calls Start

    ~Timer();
    // calls Stop
    // intentionally not virtual

    void Start(unsigned int milliseconds);
    // calls Stop and starts the timer

    void Stop();
    // stops the timer if it's running

    bool IsRunning() const { return itIsRunning; }

private:
    class Impl;
    static Impl& Instance();

    bool itIsRunning = false;
    unsigned int itsId = 0;
    Client& itsClient;
};

// Timer uses windows messages. The ITimerClient::TimerElapsed function
// is only called while the windows message loop is working.


class Timer::Client
{
public:
    virtual void TimerElapsed() = 0;

protected:
    ~Client() = default;
};

}
