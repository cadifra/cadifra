/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module App.ICmdView;

import App.CmdState;


namespace App
{

export class ICmdView
{
public:
    ICmdView(int id):
        id_(id) {}
    ICmdView(const ICmdView&) = delete;
    ICmdView& operator=(const ICmdView&) = delete;

    virtual ~ICmdView() = default;

    virtual void update(bool isEnabled, CmdState) const = 0;

    int getId() const { return id_; }

private:
    int id_;
};

}
