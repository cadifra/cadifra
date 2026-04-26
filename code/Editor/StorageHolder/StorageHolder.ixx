/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module Editor.StorageHolder;

import d1.Observer;
import d1.AutoComPtr;

import WinUtil.IReadOnlyStateChange;
import WinUtil.ISavedChange;
import WinUtil.UniqueHandle;

import Core.Main;
import Diagram;

import std;


namespace Editor
{

export class StorageHolder: private Core::IDirtyStateChange
{
public:
    StorageHolder(::Diagram::Diagram&);

    StorageHolder(const StorageHolder&) = delete;
    StorageHolder& operator=(const StorageHolder&) = delete;

    virtual ~StorageHolder();


    // functions to implement the user interface:

    void createNew();
    void open(const std::wstring& FileName, bool readOnly);
    void save();
    void saveAs(const std::wstring& FileName);
    void saveACopyAs(const std::wstring& FileName);

    bool isReadOnly() const;
    bool isNew() const;
    bool isEmbedded() const { return isEmbedded_; }

    auto getReadOnlyConnector()
        -> d1::Observer::C<WinUtil::IReadOnlyStateChange>;

    auto getSaveConnector() -> d1::Observer::C<WinUtil::ISavedChange>;

    // functions to implement IPersistFile:

    void load(const std::wstring& FileName, unsigned long dwMode);
    void save(const std::wstring& FileName, bool fRemember);
    void saveCompleted(const std::wstring& FileName);
    std::wstring getCurFile() const;


    // functions to implement IPersistStorage:

    using Stg = d1::AutoComPtr<::IStorage>;

    void initNew(Stg);
    void load(Stg);
    void save(Stg, bool IsCurrentStg);
    void saveCompleted(Stg);
    void handsOffStorage();

    // common functions

    bool isDirty() const;

    auto getDirtyStateConnector() -> d1::Observer::C<Core::IDirtyStateChange>;

private:
    enum class State
    {
        Uninitialized,
        Normal,
        NoScribble,
        HandsOffFromNormal,
        HandsOffAfterSave
    } state_{ State::Uninitialized }; // used only for IPersistStorage

    ::Diagram::Diagram& diagram_;

    bool isEmbedded_ = false;
    bool isReadOnly_ = false;
    bool isNew_ = true;
    bool isNullFile_ = false;
    std::wstring filename_;
    Stg storage_;

    WinUtil::UniqueFileHandle fileHandle_;
    std::unique_ptr<_FILETIME> lastSavedTime_;
    std::unique_ptr<Core::IDirtyMarker> dirtyMarker_; // not for embedded use

    d1::Observer::L<WinUtil::IReadOnlyStateChange> readOnlyObservers_;
    d1::Observer::L<WinUtil::ISavedChange> saveObservers_;
    d1::Observer::L<Core::IDirtyStateChange> dirtyStateObservers_;

    d1::Observer::C<Core::IDirtyStateChange> markerConnector_;

    void notifyReadOnlyObservers();
    void notifySaveObservers();
    void notifyDirtyStateObservers();

    void dirtyStateChanged(bool IsDirty) override; // from Core::IDirtyStateChange

    void updateLastSavedTime();

    void setEmbedded(bool embedded);

    void cleanUp();

    void loadXML(const HANDLE f);
    void saveXML(const HANDLE f);

    bool testForNullFile(const std::wstring& fileName, bool readOnly);

    class Reporter;

    int reporterCount_ = 0;
};

}
