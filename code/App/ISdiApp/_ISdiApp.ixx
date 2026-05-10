/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module App.ISdiApp;

import App.IApplication;
import App.CommandLine;
import App.Document;


namespace App
{

class OleInitializer
{
public:
    OleInitializer();
    OleInitializer(const OleInitializer&) = delete;
    OleInitializer& operator=(const OleInitializer&) = delete;

    ~OleInitializer(); // intentionally not virtual

    bool isInitialized() const { return itIsInitialized; }

private:
    bool itIsInitialized;
};


export class ISdiApp:
    public IApplication,
    private CommandLine::IReceiver
{
public:
    ISdiApp() = default;
    ISdiApp(const ISdiApp&) = delete;
    ISdiApp& operator=(const ISdiApp&) = delete;

    virtual ~ISdiApp();

    int WinMain(
        CommandLine&,
        WinUtil::HINSTANCE hInstance,     // handle to current instance
        WinUtil::HINSTANCE hPrevInstance, // handle to previous instance
        int nCmdShow);                    // show state of window


    virtual int init() = 0;
    // Makes any initializations and returns 0 if ok

    virtual auto createDocumentFactory(LifeTimeSupervisor&)
        -> Document::IFactory& = 0;


    //-- IApplication

    auto getMessageLoop() -> WinUtil::MessageLoop& override
    {
        return messageLoop_;
    }

    WinUtil::HINSTANCE getInstanceHandle() const override { return hInstance_; }

    //--


private:
    OleInitializer oleInitializer_;
    // oleInitializer_ has to be the first member variable!

    WinUtil::HINSTANCE hInstance_{};
    WinUtil::MessageLoop messageLoop_;
    class Quit;
};

}
