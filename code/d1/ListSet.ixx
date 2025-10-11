/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module d1.ListSet;

import std;

/*
The ListSet class works as a replacement for std::set. ListSet behaves like
std::set but its iterators work different. The ListSet iterators traverse
the container in the sequence of insertion: "begin()" refers to the entry
inserted first.

Consider the use of ListSet instead of std::set if you want to store some form
of pointers and use the memory address as the key for std::set. Because memory
allocations from the heap return rather random addresses, the behavior of the
program may change between different runs even if all program inputs are equal.

ListSet stores its entries both in a std::set and in a double linked list. The
set is used to find entries in logarithmic time and to prevent insertion of the
same value several times. The double linked list enables traversing the entries
in the sequence of insertion. The insert function always adds a new entry at
the end of the list.

Removal or insertion of elements do not invalidate any iterators except those
that refer to removed elements (like std::set).
*/

namespace d1
{

export template <class Key, class Compare = std::less<Key>>
class ListSet
{
    struct Node
    {
        Key itsKey{};
        Node* itsPrev = nullptr;
        Node* itsNext = nullptr;
        Node(const Key& k, Node* p = nullptr):
            itsKey{ k }, itsPrev{ p }
        {
        }
    };

    struct Comp
    {
        Compare itsCompare;
        Comp(const Compare& c):
            itsCompare{ c }
        {
        }
        bool operator()(const Node& x, const Node& y) const
        {
            return itsCompare(x.itsKey, y.itsKey);
        }
    };

    using Nodes = std::set<Node, Comp>;
    Nodes itsNodes;
    Node* itsFirst = nullptr;
    Node* itsLast = nullptr;

public:
    using size_type = Nodes::size_type;

    class iterator;
    class const_iterator;

    ListSet(const Compare& c = Compare{}):
        itsNodes{ Comp{ c } }
    {
    }

    template <class Iter>
    ListSet(Iter i, Iter e, const Compare& c = Compare{}):
        itsNodes{ Comp{ c } }
    {
        for (; i != e; ++i)
            insert(*i);
    }

    ListSet(const ListSet& s);

    size_type size() const { return itsNodes.size(); }

    bool empty() const { return itsFirst == 0; }

    const_iterator begin() const;
    const_iterator end() const;

    iterator begin();
    iterator end();

    void clear()
    {
        itsFirst = 0;
        itsLast = 0;
        itsNodes.clear();
    }

    auto insert(const Key& k) -> std::pair<iterator, bool>;

    template <class Iter>
    void insert(Iter i, Iter e)
    {
        for (; i != e; ++i)
            insert(*i);
    }

    auto find(const Key& k) const -> const_iterator;
    auto find(const Key& k) -> iterator;

    size_type erase(const Key& k);
};


template <class Key, class C>
class ListSet<Key, C>::iterator
{
    Node* itsNode = nullptr;
    friend class const_iterator;

public:
    using value_type = Key;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = Key*;
    using reference = Key&;

    explicit iterator(Node* n = 0):
        itsNode{ n }
    {
    }

    bool operator==(const iterator& rhs) const { return itsNode == rhs.itsNode; }

    auto& operator++()
    {
        itsNode = itsNode->itsNext;
        return *this;
    }

    auto operator++(int) // post-increment
    {
        auto tmp = *this;
        itsNode = itsNode->itsNext;
        return tmp;
    }

    auto& operator--()
    {
        itsNode = itsNode->itsPrev;
        return *this;
    }

    auto operator--(int) // post-decrement
    {
        auto tmp = *this;
        itsNode = itsNode->itsPrev;
        return tmp;
    }

    Key* operator->() const { return &itsNode->itsKey; }
    Key& operator*() const { return itsNode->itsKey; }
};


template <class Key, class C>
class ListSet<Key, C>::const_iterator
{
    const Node* itsNode = nullptr;

public:
    using value_type = Key;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = Key*;
    using reference = Key&;

    explicit const_iterator(const Node* n = 0):
        itsNode{ n }
    {
    }

    const_iterator(const iterator& it):
        itsNode{ it.itsNode }
    {
    }

    bool operator==(const const_iterator& rhs) const { return itsNode == rhs.itsNode; }

    auto& operator++()
    {
        itsNode = itsNode->itsNext;
        return *this;
    }

    auto operator++(int) // post-increment
    {
        auto tmp = *this;
        itsNode = itsNode->itsNext;
        return tmp;
    }

    auto& operator--()
    {
        itsNode = itsNode->itsPrev;
        return *this;
    }

    auto operator--(int) // post-decrement
    {
        auto tmp = *this;
        itsNode = itsNode->itsPrev;
        return tmp;
    }

    const Key* operator->() const { return &itsNode->itsKey; }
    const Key& operator*() const { return itsNode->itsKey; }
};


template <class Key, class C>
ListSet<Key, C>::ListSet(const ListSet& s):
    itsNodes{ Comp{ s.itsNodes.key_comp() } }
{
    auto i = s.begin();
    auto e = s.end();
    if (i == e)
        return;
    auto ni = itsNodes.insert(Node{ *i }).first;
    auto* n = const_cast<Node*>(&*ni);
    itsFirst = n;
    for (++i; i != e; ++i)
    {
        auto n2i = itsNodes.insert(ni, Node{ *i, n });
        auto* n2 = const_cast<Node*>(&*n2i);
        n->itsNext = n2;
        ni = n2i;
        n = const_cast<Node*>(&*ni);
    }
    itsLast = n;
}


template <class Key, class C>
auto ListSet<Key, C>::begin() const -> const_iterator
{
    return const_iterator{ itsFirst };
}


template <class Key, class C>
auto ListSet<Key, C>::end() const -> const_iterator
{
    return const_iterator{};
}


template <class Key, class C>
auto ListSet<Key, C>::begin() -> iterator
{
    return iterator{ itsFirst };
}


template <class Key, class C>
auto ListSet<Key, C>::end() -> iterator
{
    return iterator{};
}


template <class Key, class C>
auto ListSet<Key, C>::insert(const Key& k) -> std::pair<iterator, bool>
{
    auto res = itsNodes.insert(Node{ k, itsLast });
    auto* n = const_cast<Node*>(&*res.first);
    if (!res.second)
        return std::make_pair(iterator{ n }, false);
    if (!itsFirst)
        itsFirst = n;
    else
        itsLast->itsNext = n;
    itsLast = n;
    return std::make_pair(iterator{ n }, true);
}


template <class Key, class C>
auto ListSet<Key, C>::find(const Key& k) const -> const_iterator
{
    auto i = itsNodes.find(k);
    if (i == itsNodes.end())
        return const_iterator{};
    else
        return const_iterator{ &*i };
}


template <class Key, class C>
auto ListSet<Key, C>::find(const Key& k) -> iterator
{
    auto i = itsNodes.find(k);
    if (i == itsNodes.end())
        return iterator{};
    else
        return iterator{ const_cast<Node*>(&*i) };
}


template <class Key, class C>
auto ListSet<Key, C>::erase(const Key& k) -> size_type
{
    auto i = itsNodes.find(k);
    if (i == itsNodes.end())
        return 0;
    auto* n = const_cast<Node*>(&*i);
    if (n->itsNext)
        n->itsNext->itsPrev = n->itsPrev;
    if (n->itsPrev)
        n->itsPrev->itsNext = n->itsNext;
    if (itsFirst == n)
        itsFirst = n->itsNext;
    if (itsLast == n)
        itsLast = n->itsPrev;
    itsNodes.erase(i);
    return 1;
}

}
