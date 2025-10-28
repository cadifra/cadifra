/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;

import d1.types;

import std;


namespace Core
{

auto MakeStandardSelectionRestorer(const ElementSet& selection)
    -> Selection::IRestorerRef
{
    class SSR: public Selection::IRestorer
    {
        using Cont = std::vector<IElementRef>;

        Cont itsSelection;

    public:
        SSR(const ElementSet& s, d1::int32 size)
        {
            itsSelection.reserve(size);

            for (auto& m : s)
                if (m.use_count() != 1)
                    itsSelection.push_back(m);
        }

        //-- Selection::IRestorer

        void Restore(Selection::Tracker& sc, IView& v) final
        {
            const auto t = ElementSet{ begin(itsSelection), end(itsSelection) };
            v.SetSelection(sc, t);
        }
    };


    class OneSR: public Selection::IRestorer
    {
        IElementRef itsSelectedElement;

    public:
        OneSR(const IElementRef& me):
            itsSelectedElement{ me }
        {
        }

        //-- Selection::IRestorer

        void Restore(Selection::Tracker& sc, IView& v) final
        {
            auto t = ElementSet{};
            t.Insert(*itsSelectedElement.get());
            v.SetSelection(sc, t);
        }
    };


    const d1::int32 size = selection.size();

    if (size == 0)
        return nullptr;

    if (size == 1)
    {
        if (begin(selection)->use_count() == 1)
            return nullptr;
        return std::make_shared<OneSR>(*begin(selection));
    }

    return std::make_shared<SSR>(selection, size);
}

}
