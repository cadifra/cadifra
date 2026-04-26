/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module Editor.Main:Window;

import Editor.Internals;
import Editor.LicenseInfo;
import Editor.Messages;
import Editor.WindowList;

import App.Com;
import App.Dialog;
import App.ExecRegistrar;
import App.ISdiApp;

import Canvas.Base;
import Canvas.ICanvas;

import d1.Color;
import d1.Point;
import d1.Observer;

import Store.IStorable;

import View.Base;
import View.DiagramView;
import View.DiagramType;

import WinUtil.IReadOnlyStateChange;
import WinUtil.IWindow;
import WinUtil.MessageLoop;
import WinUtil.Messages;
import WinUtil.Mouse;
import WinUtil.types;
import WinUtil.Window;

import Core.Main;
import Style;

import std;


namespace Editor
{

export class Diagram;
export class Window;

class WindowImp;

class Toolbar
{
    class MessagePreProcessor;

    Window& owner_;
    WinUtil::HINSTANCE instance_ = {};
    HWND parent_ = {};

    HWND toolBarWindow_ = {};

    HWND diagramTypeControl_ = {};
    int diagramTypeControlCurrentIndex_ = 0;
    int numOfDiagramTypes_ = 0;

    HWND zoomControl_ = {};
    HWND zoomControlEditCtrl_ = {};
    bool isChangeFactorCallActive = false;
    int lastShownZoomFactor_ = 0;

    int height_ = 0;
    bool visible_ = false;

    std::unique_ptr<MessagePreProcessor> messagePreProcessor_;

public:
    static const wchar_t* showRegistryName()
    {
        return L"ShowToolbar";
    }

    Toolbar(Window& owner);

    Toolbar(const Toolbar&) = delete;
    Toolbar& operator=(const Toolbar&) = delete;

    ~Toolbar();

    void init(
        HWND parent, WinUtil::HINSTANCE instance,
        const NewWindowDefaultsProvider* dp);

    LONG height() const;
    LONG yoffset() const;

    void show(bool show);
    bool visible() const { return visible_; }
    HWND window() const { return toolBarWindow_; }

    void saveStateToRegistry() const;

    void notify(WinUtil::WM_COMMAND_Msg msg);

    void setDiagramType(View::DiagramTypeValue::T);

    void updateToFactor();

private:
    void calculateAndCacheHeight();

    void show();
    void hide();

    void restoreStateFromRegistry();

    void addButtons();

    void createEntries();
    void addEntry(int id);

    void createDiagramTypeComboBox();
    void createZoomComboBox();

    void notifyDiagramType(WinUtil::WM_COMMAND_Msg msg);
    void notifyZoom(WinUtil::WM_COMMAND_Msg msg);

    bool preProcess(MSG& msg);

    void applyFactor();
    d1::Point center() const;

    void setZoomFactor(int new_factor);
    void showZoomFactor(int factor);
    void postShowZoomFactor(int factor);

    static unsigned int showZoomFactorMessageID();

    void setFocusToOwner();

    void restoreLastShownZoomFactor();
};


class ToolbarEntry: public App::ICmdView
{
public:
    ToolbarEntry(Toolbar& tb, int id);

    ToolbarEntry(const ToolbarEntry&) = delete;
    ToolbarEntry& operator=(const ToolbarEntry&) = delete;

private:
    void update(bool isEnabled, App::CmdState) const override;

    Toolbar& toolbar_;
};


class SplitBar
{
    Window& owner_;
    bool moving_ = false;
    bool useKey_ = false;
    WinUtil::HBRUSH brush_ = {};
    WinUtil::Window window_;
    RECT dragRect_ = {};
    int splitPos_ = 0;
    bool itIsSplit = false;
    d1::Point oldCursorPos_;
    WinUtil::ProcRegistrar procReg_;

public:
    SplitBar(Window&);

    SplitBar(const SplitBar&) = delete;
    SplitBar& operator=(const SplitBar&) = delete;

    void create(HWND parent, const WinUtil::HINSTANCE& instance);

    HWND getWindowHandle() const { return window_.getWindowHandle(); }

    bool isSplit() const { return itIsSplit; }
    void setSplit(bool split) { itIsSplit = split; }

    int splitPos() const { return splitPos_; }
    void setSplitPos(int p) { splitPos_ = p; }

    void startSplitBarMove(int pos, bool useKey);
    void stopSplitBarMove(bool cancel);
    void drawSplitBarDragRect(int pos, bool start, bool stop);
    void removeSplitBar();

private:
    void onPaint(WinUtil::WM_PAINT_Msg);
    void onLButtonDown(WinUtil::WM_LBUTTONDOWN_Msg);
    void onLButtonUp(WinUtil::WM_LBUTTONUP_Msg);
    void onLButtonDbClk(WinUtil::WM_LBUTTONDBLCLK_Msg);
    void onMouseMove(WinUtil::WM_MOUSEMOVE_Msg);
    void onKillFocus(WinUtil::WM_KILLFOCUS_Msg);
    void onKeyDown(WinUtil::WM_KEYDOWN_Msg);
    void onCaptureChanged(WinUtil::WM_CAPTURECHANGED_Msg);
    void onSetCursor(WinUtil::WM_SETCURSOR_Msg);
};


class ClientView:
    private Canvas::IScrollChange,
    private Canvas::IZoomChange
{
public:
    WinUtil::Window window_;
    HWND vScrollBar_ = {};
    std::unique_ptr<Canvas::IScreenCanvas> screenCanvas_;
    std::unique_ptr<View::DiagramView> diagramView_;
    WindowImp& parent_;
    ClientView* opposite_;

    d1::Observer::C<Canvas::IScrollChange> scrollConnector_;
    d1::Observer::C<Canvas::IZoomChange> zoomConnector_;

    ClientView(Window& parent, ClientView* opposite);
    ~ClientView();

    //-- Canvas::IScrollChange

    void scrollNotification(const d1::fPoint&) override;

    //-- Canvas::IZoomChanged

    void zoomNotification(const double& zoomFactor,
        const d1::Point& fixPoint) override;

    //--
};


export class Window:
    public App::Document::IWindow,
    public Style::IElementStyleRegistryOwner
{
public:
    static auto create(
        App::LifeTimeSupervisor&,
        Diagram&,
        App::IClipboard&,
        Core::IDiagram&,
        d1::Observer::C<Core::IDirtyStateChange>,
        d1::Observer::C<WinUtil::IReadOnlyStateChange>,
        const Store::FactoryRegistry&,
        const NewWindowDefaultsProvider*)
        -> std::unique_ptr<Window>;

    using IWindow::IWindow;

    virtual auto getScreenCanvas() const -> Canvas::IScreenCanvas& = 0;

    virtual ~Window() = default;

    virtual auto getToolbar() -> Toolbar& = 0;
    virtual auto splitbar() -> SplitBar& = 0;
    virtual auto firstClientView() -> ClientView& = 0;

    virtual void setActiveClientView(ClientView*) = 0;
    virtual auto getActiveClientView() const -> ClientView* = 0;

    virtual auto getActiveDiagramView() const -> View::DiagramView* = 0;

    virtual void split() = 0;
    virtual void removeSplitBar() = 0;

    virtual void newSize() = 0;

    virtual void add(const std::shared_ptr<App::ICmdView>&) = 0;

    virtual auto getDiagramType() const -> View::DiagramTypeValue::T = 0;

    virtual auto getDiagram() -> Core::IDiagram& = 0;

    virtual auto getImp() -> WindowImp& = 0;
};


class ZoomCmdHandler
{
    class Rep;
    std::unique_ptr<Rep> rep;

public:
    ZoomCmdHandler(const Window& parent, App::ICommander&);

    ZoomCmdHandler(const ZoomCmdHandler&) = delete;
    ZoomCmdHandler& operator=(const ZoomCmdHandler&) = delete;

    ~ZoomCmdHandler(); // intentionally not virtual

    void updateToFactor();

    void dialogClosed();
};


namespace ElementStyles
{

class List;


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


class List:
    public Style::IElementStyleRegistry
{
    using This = List;

    using Entry = ListEntry;
    using Entries = std::vector<std::unique_ptr<Entry>>;

    Window& window_;
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

    explicit List(Window&);
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


class WindowImp:
    public Window,
    private App::Document::INameChange,
    private App::ISettingChanged,
    private WinUtil::MessageLoop::IPreProc,
    private Core::IDirtyStateChange,
    private Core::IUndoRedoCountChange,
    private WinUtil::IReadOnlyStateChange,
    private WinUtil::MouseInputCapturer::Control,
    private View::IDiagramTypeObserver,
    private WinUtil::WindowResizer::ResizeAlgorithm
{
    using This = WindowImp;
    using Inherited = App::Document::IWindow;

public:
    Canvas::IScreenCanvas& getScreenCanvas() const;

    virtual ~WindowImp();

    auto getToolbar() -> Toolbar&;
    auto splitbar() -> SplitBar&;
    auto firstClientView() -> ClientView&;

    void setActiveClientView(ClientView*);
    auto getActiveClientView() const -> ClientView*;

    auto getActiveDiagramView() const -> View::DiagramView*;

    void split();
    void removeSplitBar();

    void newSize();

    void add(const std::shared_ptr<App::ICmdView>&);

    auto getDiagramType() const -> View::DiagramTypeValue::T;

    auto getDiagram() -> Core::IDiagram&;

    auto getImp() -> WindowImp& { return *this; }

private:
    //-- member variables --------------------------------------------------------

    friend class ClientView;
    std::unique_ptr<ClientView> clientView1_;
    std::unique_ptr<ClientView> clientView2_;
    ClientView* activeClientView_ = nullptr;

    int windowNumber_ = 0;
    Canvas::Group test2Group_;
    HACCEL acceleratorTable_;
    bool inEvaluationMode_ = false;
    int remainingDays_ = 0;
    bool evalTimeElapsed_ = false;

    App::Commander commander_ = { *this };
    LicenseInfo::Adder licenseInfoRegistrar_;

    using CmdViews = std::vector<App::ICmdView*>;
    CmdViews cmdViews_;

    HWND HScrollBar_ = {};
    HWND sizeBox_ = {};

    std::unique_ptr<WinUtil::WindowResizer> windowResizer_;

    App::ISettingChanged::Bookkeeper settingChangeObsBookkeeper_ = { *this };

    std::unique_ptr<WindowList> windowList_;
    std::unique_ptr<ElementStyles::List> elementStylesList_;

    std::unique_ptr<Style::PropertyEditorState> propertyEditorState_;


    Diagram& document_;
    App::IClipboard& clipboard_;
    std::unique_ptr<App::ClipboardViewer> clipboardViewer_;
    Core::IDiagram& diagram_;
    std::wstring diagramName_;
    bool diagramIsDirty_ = false;
    bool diagramIsReadOnly_ = false;
    const std::unique_ptr<View::ITransferSetFormatHandler> transferSetFormatHandler_;
    d1::Observer::C<App::Document::INameChange> docNameObs_;
    d1::Observer::C<Core::IDirtyStateChange> dirtyStateObs_;
    d1::Observer::C<Core::IUndoRedoCountChange> undoRedoCountObs_;
    d1::Observer::C<WinUtil::IReadOnlyStateChange> readOnlyObs_;
    WinUtil::ProcRegistrar procReg_;
    App::ExecRegistrar execReg_;
    std::unique_ptr<Toolbar> toolBar_;
    std::unique_ptr<SplitBar> splitBar_;
    ZoomCmdHandler zoomCmdHandler_;

public:
    WindowImp(
        App::LifeTimeSupervisor& lts,
        Diagram& doc,
        App::IClipboard& clipboard,
        Core::IDiagram& diagram,
        d1::Observer::C<Core::IDirtyStateChange> dsoc,
        d1::Observer::C<WinUtil::IReadOnlyStateChange> rooc,
        const Store::FactoryRegistry& freg,
        const NewWindowDefaultsProvider* dp);

private:
    //-- ILifeTimeSupervisorClient
    void shutDown() override;

    //-- App::INumberedWindow
    void setWindowNumber(int number) override;

    //-- App::IDocumentWindow
    void changeActiveState(bool isActive) override;

    //-- App::IDocumentNameObserver
    void documentNameChanged(std::wstring) override;

    //-- WinUtil::MessageLoop::IPreProc
    bool preProcess(MSG&) override;

    //-- Core::IDirtyStateObserver
    void dirtyStateChanged(bool IsDirty) override;

    //-- Core::IUndoRedoCountObserver
    void undoRedoCountChanged(int UndoCount, int RedoCount) override;

    //-- WinUtil::IReadOnlyObserver
    void readOnlyStateChanged(bool IsReadOnly) override;

    //-- WinUtil::IMouseInputControl
    void implSetCaptureMouseInput(bool) override;

    //-- View::IDiagramTypeObserver
    void diagramTypeUpdate() override;

    //-- App::ISettingChangeObserver
    void settingChanged() override;

    //-- WinUtil::WindowResizer::ResizeAlgorithm
    HDWP callDeferWindowPos(HDWP hdwp, const RECT& oldParent,
        const RECT& newParent) override;
    int storePos() override
    {
        return 0;
    }

    //-- Style::IElementStyleRegistryOwner
    Style::IElementStyleRegistry* getElementStyleRegistry() override;

    //--

    void init(const NewWindowDefaultsProvider* dp);

    void registerCmds();
    void deleteMenuCommandViews();

    void onCreate(WinUtil::WM_CREATE_Msg);
    void onSetCursor(WinUtil::WM_SETCURSOR_Msg);
    void onActivate(WinUtil::WM_ACTIVATE_Msg);
    void onSetFocus(WinUtil::WM_SETFOCUS_Msg);
    void onDeferredSetFocus(DeferredSetFocusMsg);
    void onLicenseInfo(LicenseInfoMsg);
    void onVScroll(WinUtil::WM_VSCROLL_Msg);
    void onHScroll(WinUtil::WM_HSCROLL_Msg);
    void onMouseActivate(WinUtil::WM_MOUSEACTIVATE_Msg);
    void onShowEvalBox(ShowEvalBoxMsg);

    void onCommand(WinUtil::WM_COMMAND_Msg);

    using WM_COMMAND_Msg = WinUtil::WM_COMMAND_Msg;

    void cmdFileSave(const WM_COMMAND_Msg&);
    void cmdFileSaveAs(const WM_COMMAND_Msg&);
    void cmdFilePageSetup(const WM_COMMAND_Msg&);
    void cmdFilePrint(const WM_COMMAND_Msg&);
    void cmdFileClose(const WM_COMMAND_Msg&);
    void cmdFileNew(const WM_COMMAND_Msg&);
    void cmdFileOpen(const WM_COMMAND_Msg&);

    void cmdEditUndo(const WM_COMMAND_Msg&);
    void cmdEditRedo(const WM_COMMAND_Msg&);

    void cmdViewToolbar(const WM_COMMAND_Msg&);

    void cmdStyleEdit(const WM_COMMAND_Msg&);
    void cmdStyleImport(const WM_COMMAND_Msg&);
    void cmdStyleElementStyles(const WM_COMMAND_Msg&);

    void cmdTest1(const WM_COMMAND_Msg&);
    void cmdTest2(const WM_COMMAND_Msg&);
    void cmdTest3(const WM_COMMAND_Msg&);
    void cmdTest4(const WM_COMMAND_Msg&);
    void cmdTest5(const WM_COMMAND_Msg&);
    void cmdTest6(const WM_COMMAND_Msg&);
    void cmdTest7(const WM_COMMAND_Msg&);

    void cmdWindowNew(const WM_COMMAND_Msg&);
    void cmdWindowSplit(const WM_COMMAND_Msg&);
    void cmdWindowCascade(const WM_COMMAND_Msg&);
    void cmdWindowTileHorizontally(const WM_COMMAND_Msg&);
    void cmdWindowTileVertically(const WM_COMMAND_Msg&);
    void cmdWindowCloseAll(const WM_COMMAND_Msg&);

    void cmdHelp(const WM_COMMAND_Msg&);
    void cmdStartBrowserHome(const WM_COMMAND_Msg&);
    void cmdAbout(const WM_COMMAND_Msg&);

    void createMenuEntries();
    void setEmbeddedMenuEntries();
    void createWindowTitle();
    void setCommandState();

    void addMenu(int id);

    void handleFileException(const std::exception& e, d1::uint32 string_id,
        bool what_in_parens = false);

    void updateToZoomFactor();

    void prohibitChanges();

    void addMenuTest();
};


class ZoomDialog:
    public App::ModelessDialog,
    private App::PercentEditCtrl::IObserver
{
    using This = ZoomDialog;

    ZoomCmdHandler& owner_;

    WinUtil::WindowPlacementHandler& wp_;

    WinUtil::ProcRegistrar pr_;
    App::ExecRegistrar er_;
    Canvas::IScreenCanvas& sc_;
    HWND window_;

    App::PercentEditCtrl percentCtrl_; // keep this at last pos!!

public:
    ZoomDialog(
        ZoomCmdHandler& owner,
        App::IApplication&, Canvas::IScreenCanvas& sc, HWND w,
        WinUtil::WindowPlacementHandler&);

    void closing();

    void updateToFactor();

    void toForeground();

private:
    void changeNotification(App::PercentEditCtrl&);

    void onInitDialog(WinUtil::WM_INITDIALOG_Msg);
    void onOK(const WinUtil::WM_COMMAND_Msg&);

    void onZoom400(const WinUtil::WM_COMMAND_Msg&) { setRadioZoom(400); }
    void onZoom200(const WinUtil::WM_COMMAND_Msg&) { setRadioZoom(200); }
    void onZoom150(const WinUtil::WM_COMMAND_Msg&) { setRadioZoom(150); }
    void onZoom100(const WinUtil::WM_COMMAND_Msg&) { setRadioZoom(100); }
    void onZoom75(const WinUtil::WM_COMMAND_Msg&) { setRadioZoom(75); }
    void onZoom50(const WinUtil::WM_COMMAND_Msg&) { setRadioZoom(50); }
    void onZoom25(const WinUtil::WM_COMMAND_Msg&) { setRadioZoom(25); }

    void onFitWindow(const WinUtil::WM_COMMAND_Msg&);

    void onClose(WinUtil::WM_CLOSE_Msg);

    void onActivate(WinUtil::WM_ACTIVATE_Msg);

    void setRadioZoom(int percent);

    void updateButtons();

    void updateRadioButtons();
    void uncheckAllRadioButtons();

    int tuned(int percent) const;

    int mappedZoomFactor(int percent) const;

    void setFocusAndSelect(int edit_ctrl_id);

    void savePlacement();
    void restorePlacement();
};

}
