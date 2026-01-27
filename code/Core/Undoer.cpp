/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;


namespace Core
{

auto UndoerRef::makeNullUndoer() -> UndoerRef
{
    class NullUndoer: public Undoer
    {
    public:
        virtual bool isNull() const final { return true; }
        virtual void undoImp(Param&) final {}
        virtual void redoImp(Param&) final {}
        virtual bool merge(Undoer& u) final { return u.isNull(); }
        virtual void remove(IElement&) final {}
    };

    static std::shared_ptr<Undoer> res = std::make_shared<NullUndoer>();
    return res;
}


namespace
{
class UndoRedoReporter
{
    Undoer& u_;
    IDiagram& d_;

public:
    UndoRedoReporter(Undoer& u, IDiagram& d):
        u_{ u },
        d_{ d }
    {
        d_.beginUndoRedo(u_);
    }
    ~UndoRedoReporter()
    {
        d_.endUndoRedo(u_);
    }
};
}


void Undoer::undo(Param& p)
{
    auto r = UndoRedoReporter{ *this, p.diagram() };
    undoImp(p);
}


void Undoer::redo(Param& p)
{
    auto r = UndoRedoReporter{ *this, p.diagram() };
    redoImp(p);
}

}
