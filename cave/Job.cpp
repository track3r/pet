#include "pch.h"
#include "Job.h"
#include <thread>

#define WIN32_EXTRALEAN
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <intrin.h>

void JobRunner::init(int numThreads)
{
    _numThreads = numThreads;
}

void JobRunner::startJob(job_t& job, const std::function<void(job_t&)>& callback)
{
    job.workers = _numThreads;
    for (int i = 0; i < _numThreads; i++)
    {
        std::thread t([&job, callback]() {
          callback(job); 
        });
        t.detach();
    }
}

bool JobRunner::finished(job_t& job)
{
    return  ((int)job.counter - (int)job.numElements) / (int)job.granularity >= (int)job.workers;
}

void JobRunner::assist(job_t& job, const std::function<void(job_t&)>& callback)
{
    InterlockedAdd(&job.workers, 1);
    callback(job);
    while (!finished(job))
    {
        
    }
    LOG("elements: %i, counter %i, gran: %i workers: %i, check: %i", job.numElements, job.counter, job.granularity, (uint32_t)job.workers, ((int)job.counter - (int)job.numElements) / (int)job.granularity);
}

void JobRunner::threadFunc()
{

}

void Queue::push(jobId id)
{
    long b = _bottom;
    _data[b % MAX_QUEUED_JOBS] = id;

    // ensure the job is written before b+1 is published to other threads.
    // on x86/64, a compiler barrier is enough.
    _WriteBarrier();

    _bottom = b + 1;
}

jobId Queue::pop()
{
    long b = _bottom - 1;
    _InterlockedExchange(&_bottom, b);

    long t = _top;
    if (t <= b)
    {
        // non-empty queue
        jobId job = _data[b % MAX_QUEUED_JOBS];
        if (t != b)
        {
            // there's still more than one item left in the queue
            return job;
        }

        // this is the last item in the queue
        if (_InterlockedCompareExchange(&_top, t + 1, t) != t)
        {
            // failed race against steal operation
            job = -1;
        }

        _bottom = t + 1;
        return job;
    }
    else
    {
        // deque was already empty
        _bottom = t;
        return -1;
    }
}

jobId Queue::steal()
{
    long t = _top;

    // ensure that top is always read before bottom.
    // loads will not be reordered with other loads on x86, so a compiler barrier is enough.
    _ReadBarrier();

    long b = _bottom;
    if (t < b)
    {
        // non-empty queue
        jobId job = _data[t % MAX_QUEUED_JOBS];

        // the interlocked function serves as a compiler barrier, and guarantees that the read happens before the CAS.
        if (_InterlockedCompareExchange(&_top, t + 1, t) != t)
        {
            // a concurrent steal or pop operation removed an element from the deque in the meantime.
            return -1;
        }

        return job;
    }
    else
    {
        // empty queue
        return -1;
    }
}