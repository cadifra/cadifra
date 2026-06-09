/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Canvas.Group;

import Canvas.IElementImp;

import std;


namespace Canvas
{

class GroupImp: public IElementImp
{
public:
    GroupImp() {}

    GroupImp(const GroupImp&) = delete;
    GroupImp& operator=(const GroupImp&) = delete;

    void add(const std::shared_ptr<IElementImp>& e)
    {
        elements_.push_back(e);
    }

    //-- IElementImp

    void move(const d1::fVector& v) override
    {
        for (auto& e : elements_)
            e->move(v);
    }

    //--

private:
    std::vector<std::shared_ptr<IElementImp>> elements_;
};


export class Group
{
public:
    Group() = default;

    Group(const std::shared_ptr<GroupImp>& e):
        imp_{ e } {}

    void clear()
    {
        imp_ = {};
    }

    void move(const d1::fVector& v)
    {
        if (imp_)
            imp_->move(v);
    }

    operator bool() const { return imp_.get() != 0; }

    void add(const std::shared_ptr<IElementImp>& e)
    {
        if (not imp_)
            imp_ = std::make_shared<GroupImp>();
        imp_->add(e);
    }

private:
    std::shared_ptr<GroupImp> imp_;
};

}
