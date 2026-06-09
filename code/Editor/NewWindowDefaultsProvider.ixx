/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Editor.NewWindowDefaultsProvider;

import App.NewWindowDefaultsProvider;

namespace Editor
{

export class NewWindowDefaultsProvider:
    public App::NewWindowDefaultsProvider
{
    bool wtb_;

public:
    explicit NewWindowDefaultsProvider(bool wtb):
        wtb_{ wtb }
    {
    }
    bool withToolBar() const { return wtb_; }
};

}
