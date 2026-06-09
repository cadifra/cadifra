/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include "d1/d1verify.h"

#include <Windows.h>

export module App.ISdiApp;

import App.IApplication;
import App.CommandLine;
import App.IDocument;


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
        -> IDocument::IFactory& = 0;


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


module : private;


namespace App
{

OleInitializer::OleInitializer():
    itIsInitialized{ SUCCEEDED(::OleInitialize(NULL)) }
{
}


OleInitializer::~OleInitializer()
{
    if (itIsInitialized)
        ::OleUninitialize();
}

}


import WinUtil.ResourceLoader;
import WinUtil.MessageFilter;

import std;


namespace App
{

ISdiApp::~ISdiApp()
{
    WinUtil::MessageFilter::instance().deactivate();
}


class ISdiApp::Quit: public LifeTimeSupervisor::Client
{
    IDocument::IFactory& factory_;
    CommandLine& commandLine_;
    void shutDown() final
    {
        commandLine_.stopReceive();
        factory_.shutDown();
    }

public:
    Quit(IDocument::IFactory& f, CommandLine& cl):
        factory_{ f }, commandLine_{ cl }
    {
    }
    ~Quit() final { ::PostQuitMessage(0); }
};


int ISdiApp::WinMain(
    CommandLine& commandLine,
    HINSTANCE hInstance,     // handle to current instance
    HINSTANCE hPrevInstance, // handle to previous instance
    int nCmdShow)            // show state of window
{
    hInstance_ = hInstance;

    auto server = LifeTimeSupervisor{}; // the server LifeTimeSupervisor

    if (not oleInitializer_.isInitialized())
        return 1;

    WinUtil::ResourceLoader::instance().init(hInstance_);

    int res = init();
    if (res != 0)
        return res; // exit if init wasn't successful

    server.setClient(
        std::make_unique<Quit>(
            createDocumentFactory(server), commandLine));

    WinUtil::MessageFilter::instance().activate(0);

    commandLine.set(server, *this);

    return messageLoop_.doLoop();
}

}
