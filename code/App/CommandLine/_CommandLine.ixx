/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module App.CommandLine;

import App.LifeTimeSupervisor;

import std;


namespace App
{

export class CommandLine
{
public:
    CommandLine(const std::wstring& cmdLine);
    // Sends cmdLine to the process that is the master if there is
    // another process from the same module (exe-file). If there is
    // no master process, the calling process becomes master and cmdLine
    // is stored for later execution.
    // If this process becomes master, call set (the master access
    // right is blocked until you call set). Otherwise exit this process.

    CommandLine(const CommandLine&) = delete;
    CommandLine& operator=(const CommandLine&) = delete;

    ~CommandLine();


    enum class Status
    {
        Failed,
        Slave,
        Master
    };

    Status getStatus() const;

    class IReceiver;

    void set(LifeTimeSupervisor&, IReceiver& r);
    // r will get the cmdLine value from the constructor call and the
    // cmdLine values from other processes.

    void stopReceive();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};


class CommandLine::IReceiver
{
public:
    virtual ~IReceiver() = default;
    virtual void interpretCommandLine(const std::wstring&) = 0;
};

}
