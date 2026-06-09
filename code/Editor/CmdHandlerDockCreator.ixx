/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Editor.CmdHandlerDockCreator;

import View.Base;

namespace Editor
{

// singleton
export class CmdHandlerDockCreator: public View::ICmdHandlerDockCreator
{
public:
    static CmdHandlerDockCreator& instance();

    auto createCmdHandlerDock(View::IDefaultTask&)
        -> std::unique_ptr<View::ICmdHandlerDock>;
};

}

module : private;

import Editor.CmdHandlerDock;


namespace Editor
{

auto CmdHandlerDockCreator::instance() -> CmdHandlerDockCreator& 
{
    static auto c = CmdHandlerDockCreator{};
    return c;
}


auto CmdHandlerDockCreator::createCmdHandlerDock(View::IDefaultTask& dt)
    -> std::unique_ptr<View::ICmdHandlerDock>
{
    return std::make_unique<CmdHandlerDock>(dt);
}

}

