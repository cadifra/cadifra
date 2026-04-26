/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Canvas.Drawables;

import d1.algorithm;

import std;


namespace Canvas
{

export template <class T>
class Drawables
{
public:
    Drawables() {}

    Drawables(const Drawables&) = delete;
    Drawables& operator=(const Drawables&) = delete;

    void insert(T* t);
    void erase(T* t);

    auto begin() const { return cont_.begin(); }
    auto end() const { return cont_.end(); }

private:
    std::vector<T*> cont_;
};


template <class T>
inline void Drawables<T>::insert(T* t)
{
    constexpr auto comp = [](T* a, T* b) { return *a < *b; };

    auto it = std::ranges::lower_bound(cont_, t, comp);

    cont_.insert(it, t);
}


template <class T>
inline void Drawables<T>::erase(T* t)
{
    d1::erase_first(cont_, t);
}


export template <class T>
inline auto begin(const Drawables<T>& d)
{
    return d.begin();
}


export template <class T>
inline auto end(const Drawables<T>& d)
{
    return d.end();
}

}
