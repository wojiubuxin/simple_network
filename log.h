#ifndef LOG_H
#define LOG_H
#include <mutex>
#include <thread>
#include <condition_variable>
#include <deque>
#include <chrono>
#include <time.h>
#include <sstream>
#include <string.h>
namespace simple
{
	class log
	{
	public:
		
		~log();
		static log& getlog_instance()
		{
			static log log_instance;
			return log_instance;
		}

		struct log_data {
			std::string filename;
			std::string data;
		};

		template<typename... T >
		void log_print(const std::string& filename, T...args)
		{
			log_data temp;
			temp.filename ="/usr/local/zhu/logs/" + filename+"_"+ get_now_time_flag() + ".log";

			temp.data = get_now_time_flag(2) + log_pre_end(args...) + "\n";

			std::unique_lock<std::mutex>lock(log_mutex);
			pro_deque.push_back(std::move(temp));
			buffer_empty.notify_all();
		}

		template<typename headstr,typename... T>
		std::string log_pre_end(headstr heads, T...args)
		{
			std::stringstream ss;
			ss <<'|'<< heads;
			
			ss << log_pre_end(args...);
			return  ss.str();
		}


		template<typename T>
		std::string log_pre_end(T arg)
		{
			std::stringstream ss;
			ss << '|'<<arg;
			return ss.str();
		}

	private:
		log();
		std::mutex log_mutex;
		std::condition_variable buffer_empty;
		std::unique_ptr<std::thread>log_write;

		std::string get_now_time_flag(int flag_file_name = 1);
		int time_gap;
		bool run;

		void threadfunc();
		void stop_thread_write();

		//生产的
		std::deque<log_data>pro_deque;
		
		//要落地的
		std::deque<log_data>con_deque;


	};

	#define loger log::getlog_instance().log_print
	#define logger log::getlog_instance().log_print

}
#endif