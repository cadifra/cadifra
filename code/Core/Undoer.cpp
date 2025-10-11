/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;


namespace Core
{

auto UndoerRef::MakeNullUndoer() -> UndoerRef
{
    class NullUndoer: public Undoer
    {
    public:
        virtual bool IsNull() const final { return true; }
        virtual void UndoImp(UndoerParam&) final {}
        virtual void RedoImp(UndoerParam&) final {}
        virtual bool Merge(Undoer& u) final { return u.IsNull(); }
        virtual void Remove(IElement&) final {}
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
        d_.BeginUndoRedo(u_);
    }
    ~UndoRedoReporter()
    {
        d_.EndUndoRedo(u_);
    }
};
}


void Undoer::Undo(UndoerParam& p)
{
    auto r = UndoRedoReporter{ *this, p.Diagram() };
    UndoImp(p);
}


void Undoer::Redo(UndoerParam& p)
{
    auto r = UndoRedoReporter{ *this, p.Diagram() };
    RedoImp(p);
}

}
