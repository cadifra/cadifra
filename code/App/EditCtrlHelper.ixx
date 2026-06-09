/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module App.EditCtrlHelper;

import d1.StackFlag;

namespace App
{

export template <class EditCtrl>
class EditCtrlHelper
{
public:
    class IObserver;

    EditCtrlHelper(IObserver* o):
        observer_{ o }
    {
    }

    void notifyObserver(EditCtrl&);
    bool isNotifying() const { return itIsNotifying; }

    class SuppressNotifications;

private:
    IObserver* observer_;
    d1::StackFlag::Ref itIsNotifying;
};


template <class EditCtrl>
class EditCtrlHelper<EditCtrl>::IObserver
{
public:
    virtual void changeNotification(EditCtrl&) = 0;
};


template <class EditCtrl>
class EditCtrlHelper<EditCtrl>::SuppressNotifications
{
    EditCtrlHelper& ECH_;
    IObserver* oldObs_;

public:
    SuppressNotifications(EditCtrlHelper& ech):
        ECH_{ ech }, oldObs_{ ech.observer_ }
    {
        ech.observer_ = 0;
    }
    ~SuppressNotifications()
    {
        ECH_.observer_ = oldObs_;
    }
};


template <class EditCtrl>
void EditCtrlHelper<EditCtrl>::notifyObserver(EditCtrl& e)
{
    if (not observer_)
        return;

    auto stackFlag = d1::StackFlag{ itIsNotifying };

    observer_->changeNotification(e);
}

}
