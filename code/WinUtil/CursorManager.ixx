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
    virtual void Set(d1::HCURSOR newCursor) = 0;
    // Sets a new cursor. It's not allowed to call directly the
    // OS-function SetCursor !

    virtual d1::HCURSOR GetCursor() const = 0;
    // Returns the cursor used in the last "Set" call

    virtual d1::HCURSOR GetWaitCursor() const = 0;
    // Returns the wait cursor handle

    virtual void RestoreCursorState() = 0;
    // Used to "correct" the cursor. This is needed if someone
    // has directly called the OS-function SetCursor

    static CursorManager& Instance();

    class LengthyOperation;
    class WaitCursorSwitch;
    class InhibitWaitCursor;
    class ImmediateWaitCursor;

protected:
    ~CursorManager() {}

private:
    virtual bool Start() = 0; // Returns true if it was not running before.
    virtual bool Stop() = 0;  // Returns true if it was running before.
};


class CursorManager::WaitCursorSwitch
{
    bool itIsOn = false;

public:
    WaitCursorSwitch();
    ~WaitCursorSwitch(); // intentionally not virtual
    void On();
    void Off();
};


class CursorManager::InhibitWaitCursor
{
    bool itHasStopped{ Instance().Stop() };

public:
    InhibitWaitCursor() {}
    ~InhibitWaitCursor()
    {
        if (itHasStopped)
            Instance().Start();
    }
};


class CursorManager::LengthyOperation
{
    bool itHasStarted{ Instance().Start() };

public:
    LengthyOperation() {}
    ~LengthyOperation()
    {
        if (itHasStarted)
            Instance().Stop();
    }
};


class CursorManager::ImmediateWaitCursor: private InhibitWaitCursor
// Create an instance of this class before you show a dialog box.
{
    d1::HCURSOR itsOldCursor = {};

public:
    ImmediateWaitCursor();
    // Inhibits the WaitCursor mechanism, sets the wait cursor
    // immediately and stores the previous cursor.
    ~ImmediateWaitCursor();
    // Restores the old cursor and releases the WaitCursor mechanism.
};

}
