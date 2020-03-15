#pragma once

struct job_t
{
    volatile long counter = 0;
    volatile long done = 0;
    uint32_t numElements = 0;
    uint16_t granularity = 0;
    void* userData = nullptr;
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