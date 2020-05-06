#pragma once

struct job_t
{
    volatile long counter = 0;
    volatile long done = 0;
    
    uint32_t numElements = 0;
    uint32_t granularity = 0;
    void* userData = nullptr;
    volatile long workers = 0;
};

struct jobRunnerState_t;
class JobRunner
{
public:
    int _numThreads;
    void init(int numThreads);
    void startJob(job_t& job, const std::function<void(job_t&)>& callback);
    bool finished(job_t& job);
    void assist(job_t& job, const std::function<void(job_t&)>& callback);

private:
    void threadFunc();
    jobRunnerState_t* _state;
};

typedef uint16_t jobId;

#define MAX_QUEUED_JOBS 4096

class Queue
{
public:
    void push(jobId id);
    jobId pop();
    jobId steal();
private:
    volatile long _top = 0;
    volatile long _bottom = 0;
    jobId _data[MAX_QUEUED_JOBS] = { 0 };
};