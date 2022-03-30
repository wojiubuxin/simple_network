#ifndef CB_TIME_H
#define CB_TIME_H
#include <map>
#include <functional>
#include <chrono>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream>
#include <string>
namespace simple
{

	class cb_time
	{
	public:
		typedef std::function<void()> time_func_cb;

		~cb_time();
		static cb_time& get_cb_time_instance()
		{
			static cb_time cb_time_instance;
			return cb_time_instance;
		}
		void insert_time_cb(double gap, time_func_cb cb,  std::string cb_name);
		void del_time_cb(std::string cb_name);
	private:
		cb_time();
		struct time_data {
			double gap; //要间隔的秒数单位,例如0.5或是5秒
			time_func_cb cb;
			std::string cb_name;
		};

		//target_time要执行的时间戳
		std::multimap<int64_t, time_data> cb_list;

		//利用系统的线程调度做定时器
		std::unique_ptr<std::thread>run_cb;

		std::mutex cb_mutex;
		std::condition_variable cb_sleep;

		bool run_flag_cb;

		void threadfunc_cb();
		void stop_thread_cb();

	};

	//#展开宏可以生成字符串标记,便于删除,而且外部也不用关心传字符串进来
	#define SetTimeCB(t, f) cb_time::get_cb_time_instance().insert_time_cb(t, f , #f)
	#define KillTimeCB(f) cb_time::get_cb_time_instance().del_time_cb(#f)
}

#endif