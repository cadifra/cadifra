/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.ObjectWithID;

import Core.ObjectID;

import d1.Shared;
import d1.types;

import std;


namespace Core
{

export class ObjectWithID: public d1::Shared
{
    ObjectID iD_{ 0 };

public:
    ObjectWithID() {};

    ObjectID getID() const { return iD_; }
    void setID(ObjectID id) { iD_ = id; }

    ObjectWithID(const ObjectWithID&):
        iD_{ 0 }
    {
    }

    ObjectWithID& operator=(const ObjectWithID&) = delete;
};


export inline bool identCheck(const ObjectWithID* a, const ObjectWithID* b)
{
    return a == b;
}


export inline bool smallerID(const ObjectWithID* obj1, const ObjectWithID* obj2)
{
    return obj1->getID() < obj2->getID();
}


export template <class R, class Pred>
void printSortedIDs(std::ostream& os, const R& range, Pred pred)
{
    auto v = std::vector(cbegin(range), cend(range));

    std::ranges::sort(v, pred);

    for (auto* obj : v)
    {
        os << obj->getID().val();
        os << ",";
    }
}


export template <class R>
void printSortedIDs(std::ostream& os, const R& range)
{
    printSortedIDs(os, range, smallerID);
}

}
