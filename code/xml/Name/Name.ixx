/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module xml.Name;

import d1.types;

import std;


namespace xml
{

using std::wstring;


export class Namespace
{
    const wchar_t* const name_;
    const wchar_t* const prefix_;

public:
    Namespace(const wchar_t* name, const wchar_t* prefix):
        name_(name), prefix_(prefix) {}

    Namespace(const Namespace& rhs) = delete;
    Namespace& operator=(const Namespace& rhs) = delete;

    const wchar_t* name() const { return name_; }
    const wchar_t* prefix() const { return prefix_; }
};


export struct Name
{
    const Namespace& ns;
    const wstring name;

    Name(const xml::Namespace& ns, const wstring& name):
        ns{ ns }, name{ name } {}

    bool operator==(const Name& rhs) const
    {
        return (rhs.ns.name() == ns.name()) 
            and (rhs.name == name);
    }
};


export class Namespaces
{
    class Rep;
    std::unique_ptr<Rep> rep_;

public:
    static auto root() -> const Namespaces&;

    explicit Namespaces(const Namespaces& master);

    ~Namespaces();

    void add(d1::uint32 line_number, const wstring& prefix, const wstring& value);
    void addDefault(d1::uint32 line_number, const wstring& value);

    auto get_ns(d1::uint32 line_number, const wstring& prefix) const -> const wstring&;

    bool has_default() const;
    auto get_default() const -> const std::wstring&;

private:
    Namespaces();

    const wstring* get_ns(const wstring& prefix) const;
};

}
