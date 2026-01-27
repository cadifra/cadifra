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

    void start(unsigned int milliseconds);
    // calls Stop and starts the timer

    void stop();
    // stops the timer if it's running

    bool isRunning() const { return running_; }

private:
    class Impl;
    static Impl& instance();

    bool running_ = false;
    unsigned int id_ = 0;
    Client& client_;
};

// Timer uses windows messages. The ITimerClient::TimerElapsed function
// is only called while the windows message loop is working.


class Timer::Client
{
public:
    virtual void timerElapsed() = 0;

protected:
    ~Client() = default;
};

}
