module;

#include <Windows.h>

export module Editor.ElementStyles;

import Editor.IWindow;

import App.ModalDialog;
import App.IntEditControl;

import Core.Main;

import d1.types;
import d1.Color;

import Style.Main;
import Style.IElementStyleRegistry;

import View.DiagramView;

import WinUtil.Messages;


namespace Editor::ElementStyles
{

export class List;


class ListEntry
{
public:
    Style::ElementStyle& elementStyle_;
    d1::uint32 refcount_ = 1;
    d1::uint32 useCount_ = 0;

    explicit ListEntry(Style::ElementStyle& s):
        elementStyle_{ s }
    {
    }
};


class ListDialog:
    public App::ModalDialog,
    private App::IntEditCtrl::IObserver
{
    using This = ListDialog;

    enum ResourceConstants
    {
        ListId = lst1,

        RedId = edt1,
        GreenId = edt2,
        BlueId = edt3,

        RedTextId = stc1,
        GreenTextId = stc2,
        BlueTextId = stc3,

        LabelId = edt4,
        LabelTextId = stc4,

        ColorId = ctl1,

        ApplyId = psh1,
        NewId = psh2,
        DeleteId = psh3,

        AssignToSelectedId = chx1
    };

    using Entry = ListEntry;

    WinUtil::ProcRegistrar procReg_;
    App::ExecRegistrar execReg_;

    List& esl_;

    App::IntEditCtrl redCtrl_;
    App::IntEditCtrl greenCtrl_;
    App::IntEditCtrl blueCtrl_;

    int selection_ = -1;
    Entry* selectedEntry_ = nullptr;
    d1::Color color_;

    Core::Env& env_;


public:
    explicit ListDialog(Core::Env&, List&);

    static void applyElementStyleToSelection(
        Core::Env& e, View::DiagramView& dv, const std::wstring& specialStyleName);

private:
    void onInitDialog(WinUtil::WM_INITDIALOG_Msg);
    void onCommand(WinUtil::WM_COMMAND_Msg);
    void onDrawItem(WinUtil::WM_DRAWITEM_Msg);

    void cmdOK(const WinUtil::WM_COMMAND_Msg&);
    void cmdApply(const WinUtil::WM_COMMAND_Msg&);
    void cmdNew(const WinUtil::WM_COMMAND_Msg&);
    void cmdDelete(const WinUtil::WM_COMMAND_Msg&);

    void changeNotification(App::IntEditCtrl&);

    void selectionChanged();

    void setLabel(const std::wstring& s);
    std::wstring getLabel() const;

    void setColor(const d1::Color& c);
    d1::Color getColor() const;

    void updateColor();
    bool diagramViewHasSelection() const;
    void enableColorsAndLabel(bool enable);
    void processLabelChange();
    void processColorChange();

    void updateEntryInListOfDialog(int index, const Entry& e);
    void changeNameInListOfDialog(int index, const std::wstring& new_name);

    std::wstring getListRepresentation(const Entry&) const;

    void updateAllEntriesInListOfDialog();
    void updateDeleteButton();

    void apply(bool closing);

    void selectLast();
    void selectTextInNameEditCtrl();
};


export class List:
    public Style::IElementStyleRegistry
{
    using This = List;

    using Entry = ListEntry;
    using Entries = std::vector<std::unique_ptr<Entry>>;

    IWindow& window_;
    App::IApplication& application_;
    WinUtil::ProcRegistrar procReg_;
    Entries entries_; // owned
    HMENU stylesMenu_;
    UINT insertCount_ = 0;
    bool sorted_ = true;

public:
    //-- Style::IElementStyleRegistry

    void add(Style::ElementStyle&) override;
    void forget(Style::ElementStyle&) override;
    void updated(Style::ElementStyle&) override;

    //--

    explicit List(IWindow&);
    ~List();

private:
    void onInitMenuPopup(WinUtil::WM_INITMENUPOPUP_Msg);
    void onCommand(WinUtil::WM_COMMAND_Msg);

    Entries::iterator find(Style::ElementStyle&);

    void removeMenus();
    void showDialog();

    Entry* getEntry(const std::wstring& name) const;
    Entry* createNewEntry(Core::Env& e);

    void resetUseCounts();
    void updateUseCounts();

    bool getNameOfElementStyleOfSelection(std::wstring& name) const;
    // Returns true if the selection is non-empty and uniform, false otherwise.
    // Parameter "name" is unchanged if the return value is false. Otherwise
    // contains the name of the ElementStyle (the empty string if it's the
    // default ElementStyle).

    void sort();

    friend class ListDialog;
};

}
