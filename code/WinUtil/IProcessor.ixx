/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1assert.h"

export module WinUtil.IProcessor;

import WinUtil.IExceptionHandler;
import WinUtil.Message;


namespace WinUtil
{

export class IProcessor
{
    const bool alwaysReady_ = false;
    IExceptionHandler* exceptionHandler_ = nullptr; // may be zero, no ownership

public:
    IProcessor(bool alwaysReady):
        alwaysReady_{ alwaysReady }
    {
    }

    IProcessor(const IProcessor&) = delete;
    IProcessor& operator=(const IProcessor&) = delete;

    virtual ~IProcessor() = default;

    bool isAlwaysReady() const { return alwaysReady_; }

    void process(Message&);

    void setExceptionHandler(IExceptionHandler* eh)
    {
        exceptionHandler_ = eh;
    }

private:
    virtual void processImp(Message&) = 0;
};


void IProcessor::process(Message& msg)
{
    if (not exceptionHandler_)
    {
        processImp(msg);
        return;
    }

    D1_ASSERT(exceptionHandler_);

    try
    {
        processImp(msg);
    }
    catch (...)
    {
        exceptionHandler_->handleException();
        throw;
    }
}

}
