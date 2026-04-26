/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module;

#include <Windows.h>

export module Editor.Main:Diagram;

import Editor.Internals;
import Editor.StorageHolder;
import Editor.OLEdiagram;

import App.Com;
import App.ISdiApp;

import WinUtil.GIClasses;
import WinUtil.types;

import Core.Main;
import Diagram;

import std;


namespace Editor
{

export class Window;

class DiagramFactory;


export class Diagram:
    public App::IDocument,
    private ::Diagram::Diagram::Initializer,
    private Core::IPastePostProcessor
{
    App::IClipboard& clipboard_;
    bool itIsActive = false;
    WinUtil::HINSTANCE appInstance_ = {};
    const Store::FactoryRegistry& factoryRegistry_;
    ::Diagram::Diagram diagram_;
    StorageHolder storageHolder_;
    std::unique_ptr<OLEdiagram::OUD> OLEdiagram_;
    DiagramFactory& diagramFactory_;

public:
    //-- App::IDocument

    void shutDown() override;
    d1::CLSID getCLSID() const override;
    bool isEmbedded() const override;
    bool isNew() const override;
    void createNewWindow(const App::NewWindowDefaultsProvider*) override;

    //-- IDocumentWindowCreator

    bool askForUserClose() override;
    void changeActiveState(bool isActive) override;

private:
    //-- Store::Diagram::Initializer

    void initializeNewDiagram(::Diagram::Diagram&) override;

    //-- Core::IPastePostProcessor

    void pastePostProcessing(Core::Env&,
        const Core::ElementSet&) const override;

    //--

public:
    Diagram(
        DiagramFactory&,
        App::LifeTimeSupervisor&,
        App::IClipboard&,
        const std::wstring& name,
        bool openFile,
        bool createWindow,
        bool readOnly,
        WinUtil::HINSTANCE appInstance,
        const Store::FactoryRegistry&,
        const NewWindowDefaultsProvider*);

    bool testForReopen(const std::wstring&);

    WinUtil::IUnknown* getIUnknown();

    Window& createNewSDIwindow(const NewWindowDefaultsProvider*);

    bool isDirty() const;
    bool isReadOnly() const;
    bool save();
    bool saveAs();
    bool saveACopy();
    void pageSetup();
    void print();
    void print(
        const std::wstring& printer,
        const std::wstring& driver,
        const std::wstring& output); // print without dialog
    void userClose();

    auto getDiagramFactory() const -> DiagramFactory&
    {
        return diagramFactory_;
    }

    auto factoryRegistry() const -> const Store::FactoryRegistry&
    {
        return factoryRegistry_;
    }

private:
    void saveMetafile(const std::wstring& filename);
    void saveEnhMetafile(const std::wstring& filename);
    void savePNGfile(const std::wstring& filename);

    d1::HWND bringLastActiveWindowToTop() const; // may return 0

    std::wstring createPrintJobName() const;
};


// This class implements the COM "Class Object" for OLEdiagram objects.
// The reference count of this class does not influence the lifetime
// of the application or any other object!
// (AddRef and Release are only dummy implementations)

class OLEdiagramFactory:
    private ::IExternalConnection,
    private ::IClassFactory
{
public:
    OLEdiagramFactory(
        DiagramFactory&,
        App::IClipboard&,
        HINSTANCE appInstance,
        const Store::FactoryRegistry&);

    OLEdiagramFactory(const OLEdiagramFactory&) = delete;
    OLEdiagramFactory& operator=(const OLEdiagramFactory&) = delete;

    virtual ~OLEdiagramFactory();

    //-- IUnknown

    HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void** ppvObject) override;

    ULONG STDMETHODCALLTYPE AddRef() override;

    ULONG STDMETHODCALLTYPE Release() override;

    //-- IClassFactory

    /* [local] */ HRESULT STDMETHODCALLTYPE CreateInstance(
        /* [unique][in] */ IUnknown* pUnkOuter,
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void** ppvObject) override;

    /* [local] */ HRESULT STDMETHODCALLTYPE LockServer(
        /* [in] */ BOOL fLock) override;

    //-- IExternalConnection

    DWORD STDMETHODCALLTYPE AddConnection(
        /* [in] */ DWORD extconn,
        /* [in] */ DWORD reserved) override;

    DWORD STDMETHODCALLTYPE ReleaseConnection(
        /* [in] */ DWORD extconn,
        /* [in] */ DWORD reserved,
        /* [in] */ BOOL fLastReleaseCloses) override;

    //--

    void shutDown();

private:
    WinUtil::GIExternalConnection<> GIExternalConnection_ = { *this };
    WinUtil::GIClassFactory<> GIClassFactory_ = { *this };

    DWORD COMRegisteredID_ = 0;
    HINSTANCE appInstance_;

    DiagramFactory& diagramFactory_;
    App::IClipboard& clipboard_;

    const Store::FactoryRegistry& factoryRegistry_;
};


class DiagramFactory:
    public App::Document::IFactory
{
public:
    DiagramFactory(
        Creator&,
        App::LifeTimeSupervisor&,
        App::IClipboard&,
        WinUtil::HINSTANCE appInstance);

    Diagram* openFile(const std::wstring&, bool readOnly);
    Diagram* testForReopen(const std::wstring&);

    Diagram* newDiagram(const NewWindowDefaultsProvider*);
    void openDiagram();

    void addDiagram(Diagram&); // does not take ownership

    //-- App::IDocumentFactory

    void forget(App::IDocument&) override;
    void shutDown() override;
    bool visitDocuments(App::Document::IVisitor& v) const override;

    //--

private:
    WinUtil::HINSTANCE appInstance_;

    std::vector<Diagram*> diagrams_; // no ownership

    std::unique_ptr<OLEdiagramFactory> OleFactory_;
    App::IClipboard& clipboard_;
    Store::FactoryRegistry factoryRegistry_;

    std::wstring generateNewName() const;

    Diagram* openFileImp(const std::wstring&, bool readOnly);
};


export class EditorApp:
    public App::ISdiApp,
    private App::Document::IFactory::Creator
{
public:
    EditorApp();

    EditorApp(const EditorApp&) = delete;
    EditorApp& operator=(const EditorApp&) = delete;

    ~EditorApp();

private:
    //-- App::ISdiApp

    auto createDocumentFactory(App::LifeTimeSupervisor&)
        -> App::Document::IFactory& override;

    int init();

    //-- App::CommandLine::IReceiver

    void interpretCommandLine(const std::wstring&) override;

    //-- App::IDocumentFactoryCreator

    auto getApplication() -> App::IApplication& override;

    //--

    std::unique_ptr<DiagramFactory> factory_;
    std::unique_ptr<App::IClipboard> clipboard_;

    void interpretCommandLine(std::wistream& is);
    void openFiles(std::wistream& is);
    void executeOption(std::wistream& is);
    void print(std::wistream& is);
    void printto(std::wistream& is);
};

}
