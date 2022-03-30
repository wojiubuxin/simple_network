#include "log.h"
#include <stdio.h>
#include <iostream>
#include <stdarg.h>

using namespace simple;
log::log():time_gap(3),run(true)
{

	auto test_ptr =  std::bind(&log::threadfunc, this);
	std::unique_ptr<std::thread> temp(new std::thread(test_ptr));
	log_write=std::move(temp);
	
}

log::~log()
{
	if (run)
	{
		stop_thread_write();
	}

}

void log::stop_thread_write()
{
	{
		std::unique_lock<std::mutex>lock(log_mutex);
		run = false;
		buffer_empty.notify_all();
	}

	log_write->join();
}

//落地线程
void log::threadfunc()
{
	while (run)
	{

		{
			std::unique_lock<std::mutex>lock(log_mutex);
			while (pro_deque.empty() && run)
			{
				buffer_empty.wait_for(lock, std::chrono::seconds(time_gap));
				
			}
		
			//交换一下
			con_deque.swap(pro_deque);
		}

		//开始落地
		while ( con_deque.empty() == false )
		{
			log_data temp;
			temp = con_deque.front();
			con_deque.pop_front();
			
			FILE* write_fp = nullptr;
			write_fp = fopen(temp.filename.c_str(), "a");
			if (write_fp != nullptr)
			{
				fwrite(temp.data.c_str(), temp.data.length(), 1, write_fp);
				fflush(write_fp);
				fclose(write_fp);
			}

		}

	}
	
}

//获取时间格式,用于生成文件名和日志前缀时间戳
std::string log::get_now_time_flag(int flag_file_name)
{
	auto timeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	
	//得到秒级flag
	int64_t flag = timeNow.count() / 1000;

	//毫秒
	int millisecond = timeNow.count() - (flag * 1000);

	time_t now = flag;
	tm time_data;
	time_data=* localtime( &now);
	char timeStrBuf[32];
	char timeStrBuf_file_name[32];

	memset(timeStrBuf, 0, sizeof(timeStrBuf));
	memset(timeStrBuf_file_name, 0, sizeof(timeStrBuf_file_name));

	int tm_year = 1900 + time_data.tm_year;
	int tm_mon = time_data.tm_mon + 1;
	sprintf(timeStrBuf, "%d-%02d-%02d [%02d:%02d:%02d:%03d]", tm_year,
		tm_mon,
		time_data.tm_mday,
		time_data.tm_hour,
		time_data.tm_min,
		time_data.tm_sec,
		millisecond);

	sprintf(timeStrBuf_file_name, "%d-%02d-%02d", tm_year,
		tm_mon,
		time_data.tm_mday);

	if (flag_file_name == 1)
	{
		//2021-11-18_
		return timeStrBuf_file_name;
	}
	else
	{
		//2021-11-18 [18:22:59]
		return timeStrBuf;
	}

	
}
