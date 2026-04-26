/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Editor.CmdHandlerDock;

import View.Base;

import std;


namespace Editor
{

export class CmdHandlerDock: public View::ICmdHandlerDock
{
    using ICmdHandlerSet = std::vector<std::unique_ptr<View::ICmdHandler>>;

    ICmdHandlerSet cmdHandlers_;

public:
    //-- View::ICmdHandlerDock

    void updateEnabledCommands() override;

    //--

    CmdHandlerDock(View::IDefaultTask&);

    CmdHandlerDock(const CmdHandlerDock&) = delete;
    CmdHandlerDock& operator=(const CmdHandlerDock& rhs) = delete;

private:
    void add(std::unique_ptr<View::ICmdHandler>);
};


// singleton
export class CmdHandlerDockCreator: public View::ICmdHandlerDockCreator
{
public:
    static CmdHandlerDockCreator& instance();

    auto createCmdHandlerDock(View::IDefaultTask&)
        -> std::unique_ptr<View::ICmdHandlerDock>;
};

}
