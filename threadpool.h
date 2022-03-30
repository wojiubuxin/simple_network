#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <deque>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
namespace simple
{
	class threadpool
	{
	public:
		typedef std::function<void()> Task;

		~threadpool();
		static threadpool& getthreadpool_instance()
		{
			static threadpool threadpool_instance;
			return threadpool_instance;
		}
		void add_task(Task task);
		void stop_thread();
	private:
		threadpool();
		std::deque<Task> tasks_deque;
		std::vector<std::unique_ptr<std::thread>>threads;

		std::mutex pool_mutex;
		std::condition_variable isFull;
		std::condition_variable isEmpty;

		void run_thread();


		Task get_task();

		//容纳最大任务数
		size_t max_task_num;

		bool run;

#define threadpool_push threadpool::getthreadpool_instance().add_task
#define threadpool_stop threadpool::getthreadpool_instance().stop_thread
	};
}
#endif 