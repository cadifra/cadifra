/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:IDiagram;

import :IClub;
import :IElement;

import d1.types;


namespace Core
{

export
{
struct Env;
class ElementSet;
class IView;
class IUndoHandler;
class Undoer;
}


export class IPastePostProcessor
{
public:
    virtual void pastePostProcessing(
        Env&, const ElementSet& pasted_elements) const = 0;

protected:
    ~IPastePostProcessor() = default;
};


export class IDiagram: public IClub
{
    const IPastePostProcessor* const pastePostProcessor_; // may be zero

public:
    IDiagram(const IPastePostProcessor* p):
        pastePostProcessor_(p) {}
    IDiagram(const IDiagram&) = delete;
    IDiagram& operator=(const IDiagram&) = delete;

    virtual ~IDiagram() = default;

    auto* getPastePostProcessor() const { return pastePostProcessor_; }

    virtual void add(IView&) = 0;
    virtual void forget(IView&) = 0;
    virtual auto getActiveView() const -> IView* = 0;

    virtual auto getUndoHandler() const -> IUndoHandler& = 0;

    virtual d1::uint32 getNumOfElements() const = 0;
    // returns the number of elements, which are in this diagram. returns
    // 0, if no diagram is opened.

    virtual auto getNamedElement(const std::wstring& name) const -> IElement* = 0;

    virtual void insert(IElementRef) = 0;
    virtual void remove(IElement&) = 0;

    virtual void assignIDs() = 0;
    // assigns ObjectIDs to all elements in the diagram that have an ID of zero

    virtual void createViewElements(IElement&, bool update_views = true) = 0;

    virtual void destroyViewElements(IElement&, Selection::Tracker&, IView* active_v = 0) = 0;

    virtual void updateViews() = 0;

    virtual bool hasRunningTransaction() const = 0;
    virtual void add(Transaction&) = 0;
    virtual void forget(Transaction&) = 0;

    virtual bool hasDoingUndoer() const = 0;
    virtual void beginUndoRedo(Undoer&) = 0;
    virtual void endUndoRedo(Undoer&) = 0;

    virtual void check() const = 0;
};

}
