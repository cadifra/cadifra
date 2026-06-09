/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Editor.Diagram;

import Editor.StorageHolder;
import Editor.OLEdiagram;
import Editor.NewWindowDefaultsProvider;
import Editor.IWindow;

import App.ISdiApp;
import App.IDocument;
import App.Clipboard;

import d1.wintypes;

import WinUtil.types;

import Diagram;

import std;


namespace Editor
{

export class Diagram:
    public App::IDocument,
    private ::Diagram::Diagram::Initializer,
    private Core::IPastePostProcessor
{
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
    class Factory;

    Diagram(
        Factory&,
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

    IWindow& createNewSDIwindow(const NewWindowDefaultsProvider*);

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

    auto getDiagramFactory() const -> Factory&
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


    App::IClipboard& clipboard_;
    bool itIsActive = false;
    WinUtil::HINSTANCE appInstance_ = {};
    const Store::FactoryRegistry& factoryRegistry_;
    ::Diagram::Diagram diagram_;
    StorageHolder storageHolder_;
    std::unique_ptr<OLEdiagram> OLEdiagram_;
    Factory& diagramFactory_;
};


class Diagram::Factory:
    public App::IDocument::IFactory
{
public:
    Factory(
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
    bool visitDocuments(App::IVisitor& v) const override;

    //--

private:
    WinUtil::HINSTANCE appInstance_;

    std::vector<Diagram*> diagrams_; // no ownership

    struct OleFactory;

    std::unique_ptr<OleFactory> OleFactory_;
    App::IClipboard& clipboard_;
    Store::FactoryRegistry factoryRegistry_;

    std::wstring generateNewName() const;

    Diagram* openFileImp(const std::wstring&, bool readOnly);
};

}
