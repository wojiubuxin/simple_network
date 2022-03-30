#include<iostream>
//#include<functional>
//#include "threadpool.h"
//#include <deque>
//#include <atomic>
//#include "stdio.h"
//#include <time.h>
//#include <chrono>
#include "log.h"
#include "cb_time.h"
#include "threadpool.h"
#include "socket_er.h"
#include "epoll_er.h"
#include <thread>
//#include <sstream>
//#include <stdlib.h>
//#include <algorithm>
//#include <map>
//#include "cb_time.h"
//#include <stdio.h>

using namespace std;
using namespace simple;

struct fuckstruct
{
	int num;
	char name[10];
};
void dingshi1(int a)
{
	logger("fuck_you", "dingshi1中文",a);
}
void dingshi5()
{
	cout << 5 << "  " << endl;
	logger("fuck_you", "dingshi5");
}
void testleek()
{

}

int main()
{
	epoll_er epoll_main;
	//设置基础的参数值
	std::shared_ptr<socket_er> Acceptor(new socket_er);
	int sockser = Acceptor->createNonblocksock();
	Acceptor->setloop(&epoll_main);
	Acceptor->events_ |= socket_er::kReadEvent;
	Acceptor->setReadCallback(std::bind(&socket_er::Acceptor_handleRead, Acceptor.get()));
	epoll_main.m_channels[sockser] = Acceptor;
	epoll_main.update(EPOLL_CTL_ADD, Acceptor.get());
	epoll_main.start_epoll();
	
	while (true)
	{
		std::string ssssa;
		std::cin >> ssssa;
		if (ssssa=="q")
		{
			epoll_main.quit_ep();
			break;
		}
	}

	for (size_t i = 6; i > 0; i--)
	{
		cout << "关闭倒计时" << i << endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	
	


	/*SOCKET serversoc;
	SOCKET clientsoc;
	SOCKADDR_IN serveraddr;
	SOCKADDR_IN clientaddr;
	char buf[1024];
	
	int len=sizeof(clientaddr);
	WSADATA wsa;
	int fuckflag = WSAStartup(MAKEWORD(2, 2), &wsa);
	serversoc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9102);
	serveraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	fuckflag = _WINSOCK2API_::bind(serversoc, (SOCKADDR*)&serveraddr, sizeof(SOCKADDR));

	cout << "开始监听\n";
	
	listen(serversoc, 1);
	clientsoc = _WINSOCK2API_::accept(serversoc, (SOCKADDR*)&clientaddr, &len);
	cout << "接入者:" << clientsoc << endl;
	cout << WSAGetLastError() << endl;
	cout << fuckflag << endl;
	cout << serversoc << endl;
	int i = 0;
	while (1)
	{
		memset(buf, 0, sizeof(buf));
		int fuck=recv(clientsoc, buf, 1024, 0);
		if (fuck<=0)
		{
			cout << "客户端关闭链接:" << fuck << endl;
			break;
		}
		
		fuckstruct fuckflag;
		memset(&fuckflag, 0, sizeof(fuckflag));
		fuckflag.num = i;
		memcpy(fuckflag.name, buf, sizeof(fuckflag.name));

		cout << "服务端收到客户端发送的数据:" << buf <<"----"<< sizeof(fuckflag) << endl;;
		memset(buf, 0, sizeof(buf));
		memcpy(buf, &fuckflag, sizeof(fuckflag));
		cout << "服务端准备发送的数据:" << buf << "----" << sizeof(buf) << endl;;
		if (send(clientsoc, buf, sizeof(fuckflag) + 1, 0) <= 0)
		{
			cout << "发给客户端错误\n";
		}
		i++;
	}



	closesocket(serversoc);
	closesocket(clientsoc);
	WSACleanup();
	*/
	//tpool_stop();
	//logger("fuckyou", 123, "中文1");
	//logger("fuckyou", "中文2", 321,456,"fasdfaegf",78977,"你大爷的");
	/*SetTimeCB(1, dingshi1);
	SetTimeCB(1, dingshi10);
	SetTimeCB(2, dingshi2);
	SetTimeCB(2, dingshi10);
	SetTimeCB(5, dingshi5);
	SetTimeCB(5, dingshi10);
	KillTimeCB(dingshi2);*/
//	cout <<std::this_thread::get_id()<< " 这个线程要先出来的 "<< get_time().c_str()<< typeid(test_time).name() << endl;
//	cout << #test_time << endl;

	/*clock_t  now = clock();
	//cout <<  << endl;

	std::multimap<int, int >aaa_2;
	aaa_2.insert(std::make_pair(0, 1));
	aaa_2.insert(std::make_pair(1, 2));
	aaa_2.insert(std::make_pair(1, 1));
	aaa_2.insert(std::make_pair(2, 1));
	int64_t c_time= chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
	*/
	/*for (auto iter = aaa_2.begin(); iter != aaa_2.end();)
	{
		
		if (iter->first==1 && iter->second==1)
		{
			aaa_2.erase(iter++);

		}
		else
		{
			iter++;
		}
	}*/
	//cout << c_time << endl;
	//simple::log::getlog_instance().log_print();
	//simple::log::getlog_instance()
//	simple::log::getlog_instance().destory();
	//simple::log::log_print();
	//simple::log::destory();
	//loger->log_print("diuleiloumu", "123", 456);
	//loger->destory();
	//log << dsa << "123";
	/*struct data_fuck {
		std::string filename;
		std::string data;

	};
	
	data_fuck aaa;
	data_fuck bbb;
	aaa.filename = "aaa.log";
	aaa.data = "this is aaa";
	std::string filename;
	filename = "1222";
	ssss(filename);*/
	/*bbb.filename = "bbb.log";
	bbb.data = "this is bbb";
	
	std::deque<data_fuck>test;
	test.push_back(aaa);
	test.push_back(bbb);

	while (test.empty() == false)
	{
		data_fuck temp;
		temp = test.front();
		test.pop_front();


		FILE* write_fp = nullptr;
		errno_t error;
		error = fopen_s(&write_fp, temp.filename.c_str(), "a");
		if (error == 0)
		{
			fwrite(temp.data.c_str(), strlen(temp.data.c_str()), 1, write_fp);
			fflush(write_fp);
			fclose(write_fp);
		}
	}*/
//std::this_thread::sleep_for(std::chrono::seconds(6));
//	system("pause");
	return 0;
}


/*
static int test_parm = 0;
union ss
{

	int a;
	double b;
};
int print(int a)
{
	cout << "test\n";
	//Sleep(10);
	return 1;
	//	printf("线程%d号，计算结果为%d,耗时为%d\n", std::this_thread::get_id(), a, ends);
}

int printString(int a, int b, int c)
{
	cout << a << "----" << b << endl;
	cout << a << "----" << c;
	cout << endl;
	return 1;
}

typedef std::function<void()> Task;
void test(Task task)
{
	if (task)
	{
		task();
	}
}

void test_cl(int task)
{
	auto sss2 = move(task);
	cout << sss2 << endl;
	cout << "________\n";
}
struct aaaaa
{
	int a;
};
std::string get_time()
{
	time_t now = time(NULL);
	tm time_data;
	localtime_s(&time_data, &now);
	char timeStrBuf[256];
	memset(timeStrBuf, 0, sizeof(timeStrBuf));
	int tm_year = 1900 + time_data.tm_year;
	int tm_mon = time_data.tm_mon + 1;
	sprintf_s(timeStrBuf, "%d-%d-%d [%d:%d:%d] ", tm_year,
		tm_mon,
		time_data.tm_mday,
		time_data.tm_hour,
		time_data.tm_min,
		time_data.tm_sec);
	return timeStrBuf;
}

void ppppp()
{
	cout << std::this_thread::get_id() << endl;
}

class testop
{
public:
	testop() {};
	~testop() {};
	int flag;
	int flag_2;
	bool operator < (const testop& b) const
	{
		return flag < b.flag;
	}

};

class cmp
{
public:
	bool operator()(const double &a, const double &b)
	{
		return a < b;
	}
};



void dingshi5()
{
	cout << 5 << "  " << get_time().c_str() << endl;
	logger("fuck_you", "dingshi5");
}

 void  dingshi10()
{
	cout << 10 << "  " << get_time().c_str() << endl;
	logger("fuck_you", "dingshi10");
}

void dingshi2()
{
	cout << 2222222 << "  " << get_time().c_str() << endl;
	logger("fuck_you", "dingshi22222222");
}
*/




