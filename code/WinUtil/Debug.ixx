/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil:Debug;

import std;


namespace WinUtil
{

export class DebugEnv
{
public:
    static DebugEnv& Inst(); // singleton


    virtual std::string GetString(
        const std::string& section,
        const std::string& key) = 0;
    // returns "" if section\key doesn't exist


    virtual int GetInt(
        const std::string& section,
        const std::string& key) = 0;
    // returns 0 if section\key doesn't exist


protected:
    ~DebugEnv() = default;
};


export class DebugOstream: public std::ostream
{
    using inherited = std::ostream;

public:
    class Buf;

    DebugOstream(std::unique_ptr<Buf> buf);
    virtual ~DebugOstream();

private:
    std::unique_ptr<Buf> itsBuf;
};


// Use the global object dout to send messages to a debugger:
//
// #ifdef _DEBUG
// WinUtil::dout << "Hello debugger!" << std::endl;
// #endif
//
// The _DEBUG switch is optional. If there isn't any debugger
// available, the dout-statement will not produce any output
// but it will consume runtime.
//
export extern DebugOstream dout;

}
