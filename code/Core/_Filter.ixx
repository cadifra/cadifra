export module Core.Filter;

import Core.Main;

import std;

namespace Core
{

export template <class T>
    requires std::derived_from<T, IElement>
class Filter: public IFilter
{
    bool pass(const VIPointable& ps) const override
    {
        using CVE = const IViewElement;
        auto ve = dynamic_cast<CVE*>(&ps);
        if (ve)
        {
            auto& me = ve->element();
            if (dynamic_cast<T*>(&me))
                return true;
        }
        return false;
    }
};

}
