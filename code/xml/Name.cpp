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

    void add(d1::uint32 line_number, const wstring& prefix, const wstring& value);
};


void C::Rep::add(d1::uint32 line_number, const wstring& prefix, const wstring& value)
{
    const bool inserted = map_.insert(std::make_pair(prefix, value)).second;

    if (not inserted)
        throw Exceptions::DuplicateNamespace(line_number);
}


C::Namespaces():
    rep_{ std::make_unique<Rep>(nullptr) }
{
}


C::Namespaces(const Namespaces& master):
    rep_{ std::make_unique<Rep>(&master) }
{
}


auto C::root() -> const Namespaces&
{
    static Namespaces root;
    return root;
}


C::~Namespaces() = default;


void C::add(d1::uint32 line_number, const wstring& prefix, const wstring& value)
{
    if ((prefix.size() >= 3) and
        (prefix[0] == 'x' or prefix[0] == 'X') and
        (prefix[1] == 'm' or prefix[1] == 'M') and
        (prefix[2] == 'l' or prefix[2] == 'L'))
        throw Exceptions::PrefixXMLnotAllowed{ line_number };
    rep_->add(line_number, prefix, value);
}


void C::addDefault(d1::uint32 line_number, const wstring& value)
{
    if (has_default())
        throw Exceptions::DuplicateNamespace{ line_number };

    rep_->has_default_ = true;
    rep_->default_ = value;
}


auto C::get_ns(const wstring& prefix) const -> const wstring*
{
    if (auto i = rep_->map_.find(prefix); i != end(rep_->map_))
        return &i->second;

    if (rep_->master_)
        return rep_->master_->get_ns(prefix);

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
    return rep_->has_default_;
}


auto C::get_default() const -> const std::wstring&
{
    return rep_->default_;
}

}
