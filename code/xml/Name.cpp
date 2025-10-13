/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

module xml.Name;

import xml.Exceptions;

import std;


namespace xml
{

namespace
{
using C = Namespaces;
}

using std::wstring;


class C::Rep
{
public:
    using Map = std::map<wstring, wstring>;
    Map map_;
    bool has_default_ = false;
    std::wstring default_;
    const Namespaces* master_;

    explicit Rep(const Namespaces* m):
        master_{ m }
    {
    }

    void Add(d1::uint32 line_number, const wstring& prefix, const wstring& value);
};


void C::Rep::Add(d1::uint32 line_number, const wstring& prefix, const wstring& value)
{
    const bool inserted = map_.insert(std::make_pair(prefix, value)).second;

    if (!inserted)
        throw Exceptions::DuplicateNamespace(line_number);
}


C::Namespaces():
    rep{ std::make_unique<Rep>(nullptr) }
{
}


C::Namespaces(const Namespaces& master):
    rep{ std::make_unique<Rep>(&master) }
{
}


auto C::Root() -> const Namespaces&
{
    static Namespaces root;
    return root;
}


C::~Namespaces() = default;


void C::Add(d1::uint32 line_number, const wstring& prefix, const wstring& value)
{
    if ((prefix.size() >= 3) &&
        (prefix[0] == 'x' || prefix[0] == 'X') &&
        (prefix[1] == 'm' || prefix[1] == 'M') &&
        (prefix[2] == 'l' || prefix[2] == 'L'))
        throw Exceptions::PrefixXMLnotAllowed{ line_number };
    rep->Add(line_number, prefix, value);
}


void C::AddDefault(d1::uint32 line_number, const wstring& value)
{
    if (has_default())
        throw Exceptions::DuplicateNamespace{ line_number };

    rep->has_default_ = true;
    rep->default_ = value;
}


auto C::get_ns(const wstring& prefix) const -> const wstring*
{
    auto i = rep->map_.find(prefix);

    if (i != end(rep->map_))
        return &i->second;

    if (rep->master_)
        return rep->master_->get_ns(prefix);

    return nullptr;
}


auto C::get_ns(d1::uint32 n, const wstring& prefix) const -> const wstring&
{
    if (auto* r = get_ns(prefix))
        return *r;

    throw xml::Exceptions::NamespaceForPrefixMissing{ n, prefix };
}


bool C::has_default() const
{
    return rep->has_default_;
}


auto C::get_default() const -> const std::wstring&
{
    return rep->default_;
}

}
