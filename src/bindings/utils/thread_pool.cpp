#include "thread_pool.hpp"

void ThreadPool::start_thread()
{
    while (!done)
    {
        function_wrapper task;
        if (work_queue.try_pop(task))
        {
            task();
        }
        else
        {
            std::this_thread::yield();
        }
    }
}

ThreadPool::ThreadPool(size_t thread_count) : done(false), joiner(threads)
{
    try
    {
        for (size_t i = 0; i < thread_count; ++i)
        {
            threads.push_back(
                std::thread(&ThreadPool::start_thread, this));
        }
    }
    catch (...)
    {
        done = true;
        throw;
    }
}

ThreadPool::~ThreadPool()
{
    done = true;
}

template <typename FunctionType>
std::future<typename std::result_of<FunctionType()>::type>
ThreadPool::submit(FunctionType f)
{
    typedef typename std::result_of<FunctionType()>::type result_type;
    std::packaged_task<result_type()> task(std::move(f));
    std::future<result_type> res(task.get_future());
    work_queue.push(std::move(task));
    return res;
}
