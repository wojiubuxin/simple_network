#include "threadpool.h"
#include <iostream>
using namespace simple;
threadpool::threadpool():max_task_num(100000), run(true)
{
	int core_num = 4;
	threads.reserve(core_num);

	for (int i = 0; i < core_num; i++)
	{
		auto test_ptr = std::bind(&threadpool::run_thread, this);
		threads.emplace_back(new std::thread(test_ptr));

	}
}

threadpool::~threadpool()
{
	if (run)
	{
		stop_thread();
	}
}

void threadpool::stop_thread()
{
	{
		std::unique_lock<std::mutex>lock(pool_mutex);
		run = false;
		isFull.notify_all();
		isEmpty.notify_all();
	}

	for (auto& thr : threads)
	{
		thr->join();
	}
}

void threadpool::run_thread()
{
	while (run)
	{
		Task task(get_task());
		if (task)
		{
			task();
		}
	}

}

threadpool::Task threadpool::get_task()
{
	std::unique_lock<std::mutex>lock(pool_mutex);
	while (tasks_deque.empty() && run)
	{
		//没任务就候着
		isEmpty.wait(lock);
	}
	Task one_task;
	if ( !tasks_deque.empty() )
	{
		one_task = tasks_deque.front();
		tasks_deque.pop_front();

		//通知接受任务的线程继续收纳
		isFull.notify_one();
	}
	return one_task;
}

void threadpool::add_task(Task task)
{
	std::unique_lock<std::mutex>lock(pool_mutex);
	while (tasks_deque.size() >= max_task_num && run)
	{
		//任务多了,先停止接收
		isFull.wait(lock);
	}

	if ( run )
	{
		tasks_deque.emplace_back(std::move(task));
		isEmpty.notify_one();
	}


}