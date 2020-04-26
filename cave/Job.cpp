#include "pch.h"
#include "Job.h"
#include <thread>

#define WIN32_EXTRALEAN
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

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
    return  (job.numElements - job.counter) / job.granularity >= (uint32_t)job.workers;
}

void JobRunner::assist(job_t& job, const std::function<void(job_t&)>& callback)
{
    InterlockedAdd(&job.workers, 1);
    callback(job);
    while (!finished(job))
    {
        
    }
}

void JobRunner::threadFunc()
{

}