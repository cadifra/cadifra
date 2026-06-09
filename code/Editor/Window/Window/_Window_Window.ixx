/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module Editor.Window:Window;

import Editor.LicenseInfo;
import Editor.Messages;
import Editor.WindowList;
import Editor.NewWindowDefaultsProvider;
import Editor.IWindow;
import Editor.ElementStyles;
import Editor.ZoomCmdHandler;
import Editor.Diagram;
import Editor.Toolbar;

import App.LengthEditControl;
import App.Clipboard;
import App.Commander;
import App.ExecRegistrar;
import App.IDocument;

import Canvas.ScreenCanvas;
import Canvas.Group;
import Canvas.Scroller;
import Canvas.IZoomChange;

import Core.IDirtyMarker;
import Core.IUndoRedoCountChange;

import d1.Observer;

import Store.IStorable;

import View.DiagramView;
import View.ITransferSetFormatHandler;
import View.CanvasView;

import WinUtil.IReadOnlyStateChange;
import WinUtil.MessageLoop;
import WinUtil.Messages;
import WinUtil.Mouse;
import WinUtil.types;
import WinUtil.WindowResizer;

import StyleEdit.PropertyEditorState;


namespace Editor
{

export class Window:
    public IWindow,
    private App::IDocument::INameChange,
    private App::ISettingChanged,
    private WinUtil::MessageLoop::IPreProc,
    private Core::IDirtyStateChange,
    private Core::IUndoRedoCountChange,
    private WinUtil::IReadOnlyStateChange,
    private WinUtil::MouseInputCapturer::Control,
    private View::IDiagramTypeObserver,
    private WinUtil::WindowResizer::ResizeAlgorithm
{
    using This = Window;
    using Inherited = IWindow;

public:
    using IWindow::IWindow;

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

    Canvas::IScreenCanvas& getScreenCanvas() const;

    virtual ~Window();

    auto getToolbar() -> Toolbar&;

    class SplitBar;

    void split();
    auto splitbar() -> SplitBar&;
    void removeSplitBar();
    
    class ClientView;

    auto firstClientView() -> ClientView&;
    void setActiveClientView(ClientView*);
    auto getActiveClientView() const -> ClientView*;

    auto getActiveDiagramView() const -> View::DiagramView*;


    void newSize();

    void add(const std::shared_ptr<App::ICmdView>&);

    auto getDiagramType() const -> View::DiagramTypeValue::T;

    auto getDiagram() -> Core::IDiagram&;

private:
    //-- member variables --------------------------------------------------------

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

    std::unique_ptr<StyleEdit::PropertyEditorState> propertyEditorState_;


    Diagram& document_;
    App::IClipboard& clipboard_;
    std::unique_ptr<App::ClipboardViewer> clipboardViewer_;
    Core::IDiagram& diagram_;
    std::wstring diagramName_;
    bool diagramIsDirty_ = false;
    bool diagramIsReadOnly_ = false;
    const std::unique_ptr<View::ITransferSetFormatHandler> transferSetFormatHandler_;
    d1::Observer::C<App::IDocument::INameChange> docNameObs_;
    d1::Observer::C<Core::IDirtyStateChange> dirtyStateObs_;
    d1::Observer::C<Core::IUndoRedoCountChange> undoRedoCountObs_;
    d1::Observer::C<WinUtil::IReadOnlyStateChange> readOnlyObs_;
    WinUtil::ProcRegistrar procReg_;
    App::ExecRegistrar execReg_;
    std::unique_ptr<Toolbar> toolBar_;
    std::unique_ptr<SplitBar> splitBar_;
    ZoomCmdHandler zoomCmdHandler_;

public:
    Window(
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

}
