/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil.IExceptionHandler;


namespace WinUtil
{

export class IExceptionHandler
{
public:
    virtual void handleException() = 0;
    // PRECONDITION: uncaught_exception() == TRUE

protected:
    ~IExceptionHandler() = default;
};

}
