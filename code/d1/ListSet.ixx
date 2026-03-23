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
        Key key_{};
        Node* prev_ = nullptr;
        Node* next_ = nullptr;
        Node(const Key& k, Node* p = nullptr):
            key_{ k }, prev_{ p }
        {
        }
    };

    struct Comp
    {
        Compare compare_;
        Comp(const Compare& c):
            compare_{ c }
        {
        }
        bool operator()(const Node& x, const Node& y) const
        {
            return compare_(x.key_, y.key_);
        }
    };

    using Nodes = std::set<Node, Comp>;
    Nodes nodes_;
    Node* first_ = nullptr;
    Node* last_ = nullptr;

public:
    using size_type = Nodes::size_type;

    class iterator;
    class const_iterator;

    ListSet(const Compare& c = Compare{}):
        nodes_{ Comp{ c } }
    {
    }

    template <class Iter>
    ListSet(Iter i, Iter e, const Compare& c = Compare{}):
        nodes_{ Comp{ c } }
    {
        for (; i != e; ++i)
            insert(*i);
    }

    ListSet(const ListSet& s);

    size_type size() const { return nodes_.size(); }

    bool empty() const { return first_ == 0; }

    const_iterator begin() const;
    const_iterator end() const;

    iterator begin();
    iterator end();

    void clear()
    {
        first_ = 0;
        last_ = 0;
        nodes_.clear();
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
    Node* node_ = nullptr;
    friend class const_iterator;

public:
    using value_type = Key;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = Key*;
    using reference = Key&;

    explicit iterator(Node* n = 0):
        node_{ n }
    {
    }

    bool operator==(const iterator& rhs) const { return node_ == rhs.node_; }

    auto& operator++()
    {
        node_ = node_->next_;
        return *this;
    }

    auto operator++(int) // post-increment
    {
        auto tmp = *this;
        node_ = node_->next_;
        return tmp;
    }

    auto& operator--()
    {
        node_ = node_->prev_;
        return *this;
    }

    auto operator--(int) // post-decrement
    {
        auto tmp = *this;
        node_ = node_->prev_;
        return tmp;
    }

    Key* operator->() const { return &node_->key_; }
    Key& operator*() const { return node_->key_; }
};


template <class Key, class C>
class ListSet<Key, C>::const_iterator
{
    const Node* node_ = nullptr;

public:
    using value_type = Key;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = Key*;
    using reference = Key&;

    explicit const_iterator(const Node* n = 0):
        node_{ n }
    {
    }

    const_iterator(const iterator& it):
        node_{ it.node_ }
    {
    }

    bool operator==(const const_iterator& rhs) const { return node_ == rhs.node_; }

    auto& operator++()
    {
        node_ = node_->next_;
        return *this;
    }

    auto operator++(int) // post-increment
    {
        auto tmp = *this;
        node_ = node_->next_;
        return tmp;
    }

    auto& operator--()
    {
        node_ = node_->prev_;
        return *this;
    }

    auto operator--(int) // post-decrement
    {
        auto tmp = *this;
        node_ = node_->prev_;
        return tmp;
    }

    const Key* operator->() const { return &node_->key_; }
    const Key& operator*() const { return node_->key_; }
};


template <class Key, class C>
ListSet<Key, C>::ListSet(const ListSet& s):
    nodes_{ Comp{ s.nodes_.key_comp() } }
{
    auto i = s.begin();
    auto e = s.end();
    if (i == e)
        return;
    auto ni = nodes_.insert(Node{ *i }).first;
    auto* n = const_cast<Node*>(&*ni);
    first_ = n;
    for (++i; i != e; ++i)
    {
        auto n2i = nodes_.insert(ni, Node{ *i, n });
        auto* n2 = const_cast<Node*>(&*n2i);
        n->next_ = n2;
        ni = n2i;
        n = const_cast<Node*>(&*ni);
    }
    last_ = n;
}


template <class Key, class C>
auto ListSet<Key, C>::begin() const -> const_iterator
{
    return const_iterator{ first_ };
}


template <class Key, class C>
auto ListSet<Key, C>::end() const -> const_iterator
{
    return const_iterator{};
}


template <class Key, class C>
auto ListSet<Key, C>::begin() -> iterator
{
    return iterator{ first_ };
}


template <class Key, class C>
auto ListSet<Key, C>::end() -> iterator
{
    return iterator{};
}


template <class Key, class C>
auto ListSet<Key, C>::insert(const Key& k) -> std::pair<iterator, bool>
{
    auto res = nodes_.insert(Node{ k, last_ });
    auto* n = const_cast<Node*>(&*res.first);
    if (not res.second)
        return { iterator{ n }, false };
    if (not first_)
        first_ = n;
    else
        last_->next_ = n;
    last_ = n;
    return { iterator{ n }, true };
}


template <class Key, class C>
auto ListSet<Key, C>::find(const Key& k) const -> const_iterator
{
    auto i = nodes_.find(k);
    if (i == nodes_.end())
        return const_iterator{};
    else
        return const_iterator{ &*i };
}


template <class Key, class C>
auto ListSet<Key, C>::find(const Key& k) -> iterator
{
    auto i = nodes_.find(k);
    if (i == nodes_.end())
        return iterator{};
    else
        return iterator{ const_cast<Node*>(&*i) };
}


template <class Key, class C>
auto ListSet<Key, C>::erase(const Key& k) -> size_type
{
    auto i = nodes_.find(k);
    if (i == nodes_.end())
        return 0;
    auto* n = const_cast<Node*>(&*i);
    if (n->next_)
        n->next_->prev_ = n->prev_;
    if (n->prev_)
        n->prev_->next_ = n->next_;
    if (first_ == n)
        first_ = n->next_;
    if (last_ == n)
        last_ = n->prev_;
    nodes_.erase(i);
    return 1;
}

}
