/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Core.Main:FollowUpJob;

import d1.Shared;

import std;


namespace Core
{

export {
struct Env;
}


export class IFollowUpJob
{
public:
    virtual void doFollowUpJob(Env& e) = 0;

protected:
    ~IFollowUpJob() = default;
};


export class FollowUpJob // has value semantics
{
    using SharedPtr = std::shared_ptr<d1::Shared>;

    IFollowUpJob* job_ = nullptr;
    SharedPtr shared_; // controls lifetime

public:
    FollowUpJob() {}

    FollowUpJob(IFollowUpJob* j, const SharedPtr& r):
        job_{ j }, shared_{ r }
    {
    }

    template <class T>
    FollowUpJob(T* t):
        job_{ t }, shared_{ t->shared_from_this() }
    {
    }

    bool operator==(const FollowUpJob& rhs) const
    {
        return rhs.job_ == job_;
    }

    void Do(Env& e)
    {
        if (job_)
            job_->doFollowUpJob(e);
    }
};

}
