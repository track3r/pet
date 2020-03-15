#include "pch.h"
#include "Job.h"
#include <thread>

void JobRunner::init(int numThreads)
{
    _numThreads = numThreads;
}

void JobRunner::startJob(job_t& job, const std::function<void(job_t&)>& callback)
{
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
    //assert(job.done <= (int)job.numElements);
    return job.done >= (int)job.numElements;
}

void JobRunner::assist(job_t& job, const std::function<void(job_t&)>& callback)
{
    callback(job);
    while (!finished(job))
    {
        
    }
}

void JobRunner::threadFunc()
{

}