 /*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module WinUtil.CursorManager;

import d1.wintypes;


namespace WinUtil
{

export class CursorManager
{
public:
    virtual void set(d1::HCURSOR newCursor) = 0;
    // Sets a new cursor. It's not allowed to call directly the
    // OS-function SetCursor !

    virtual d1::HCURSOR getCursor() const = 0;
    // Returns the cursor used in the last "Set" call

    virtual d1::HCURSOR getWaitCursor() const = 0;
    // Returns the wait cursor handle

    virtual void restoreCursorState() = 0;
    // Used to "correct" the cursor. This is needed if someone
    // has directly called the OS-function SetCursor

    static CursorManager& instance();

    class LengthyOperation;
    class WaitCursorSwitch;
    class InhibitWaitCursor;
    class ImmediateWaitCursor;

protected:
    ~CursorManager() {}

private:
    virtual bool start() = 0; // Returns true if it was not running before.
    virtual bool stop() = 0;  // Returns true if it was running before.
};


class CursorManager::WaitCursorSwitch
{
    bool isOn_ = false;

public:
    WaitCursorSwitch();
    ~WaitCursorSwitch(); // intentionally not virtual
    void on();
    void off();
};


class CursorManager::InhibitWaitCursor
{
    bool hasStopped_{ instance().stop() };

public:
    InhibitWaitCursor() {}
    ~InhibitWaitCursor()
    {
        if (hasStopped_)
            instance().start();
    }
};


class CursorManager::LengthyOperation
{
    bool hasStarted_{ instance().start() };

public:
    LengthyOperation() {}
    ~LengthyOperation()
    {
        if (hasStarted_)
            instance().stop();
    }
};


class CursorManager::ImmediateWaitCursor: private InhibitWaitCursor
// Create an instance of this class before you show a dialog box.
{
    d1::HCURSOR oldCursor_ = {};

public:
    ImmediateWaitCursor();
    // Inhibits the WaitCursor mechanism, sets the wait cursor
    // immediately and stores the previous cursor.
    ~ImmediateWaitCursor();
    // Restores the old cursor and releases the WaitCursor mechanism.
};

}
