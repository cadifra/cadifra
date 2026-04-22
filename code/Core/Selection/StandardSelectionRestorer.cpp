/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module Core;

import d1.types;

import std;


namespace Core
{

auto makeStandardSelectionRestorer(const ElementSet& selection)
    -> Selection::IRestorerRef
{
    class SSR: public Selection::IRestorer
    {
        using Cont = std::vector<IElementRef>;

        Cont selection_;

    public:
        SSR(const ElementSet& s, d1::int32 size)
        {
            selection_.reserve(size);

            auto f = [](auto& m) { return m.use_count() != 1; };

            for (auto& m : s | std::views::filter(f))
                selection_.push_back(m);
        }

        //-- Selection::IRestorer

        void restore(Selection::Tracker& sc, IView& v) final
        {
            const auto t = ElementSet{ cbegin(selection_), cend(selection_) };
            v.setSelection(sc, t);
        }
    };


    class OneSR: public Selection::IRestorer
    {
        IElementRef selectedElement_;

    public:
        OneSR(const IElementRef& me):
            selectedElement_{ me }
        {
        }

        //-- Selection::IRestorer

        void restore(Selection::Tracker& sc, IView& v) final
        {
            auto t = ElementSet{};
            t.insert(*selectedElement_.get());
            v.setSelection(sc, t);
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
