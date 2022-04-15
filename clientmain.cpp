#include <iostream>
#include "log.h"
#include "cb_time.h"
#include "threadpool.h"
#include "socket_er.h"
#include "epoll_er.h"
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include "msg.h"
#include <cstdlib>
#include <ctime>
using namespace std;
using namespace simple;
std::mutex pmutex;
typedef std::map<int, std::shared_ptr<socket_er>>    fuckmap;
fuckmap       m_channels;

//建立连接
void te()
{
	std::unique_lock<std::mutex>lock(pmutex);
	std::shared_ptr<socket_er>temp_client(new socket_er);
	int fla=temp_client->createNonblocksock_client();
	temp_client->setMessageCallback(std::bind(&socket_er::message_er, temp_client.get()));
	m_channels[fla] = temp_client;

}

void fa2(socket_er* tem)
{
	msg_buy data;
	data.itemid = 10086;
	data.count = 50;
	//for (size_t i = 0; i < 2; i++)
	{
		tem->send(BUY_ASK, &data, sizeof(data));
	}
	
	
}

void fa3(socket_er* tem)
{
	msg_mission data;
	memcpy(data.data, "已转职", sizeof(data.data));
	for (size_t i = 0; i < 10000; i++)
	{
		tem->send(MISSION_ASK, &data, sizeof(data));
	}
}

void fa1()
{
	std::unique_lock<std::mutex>lock(pmutex);
	for (auto iter = m_channels.begin(); iter != m_channels.end();iter++)
	{
		//int fla = rand()%5+1;
		//SetTimeCB(fla, std::bind(&fa2, iter->second.get()));
		//fla = rand() % 5 + 1;
		//SetTimeCB(fla, std::bind(&fa3, iter->second.get()));
		//fa2(iter->second.get());
		fa3(iter->second.get());
	}


}

void shou1()
{
	std::unique_lock<std::mutex>lock(pmutex);
	for (auto iter = m_channels.begin(); iter != m_channels.end(); iter++)
	{
		//iter->second.get()->read_er();
		char temp[10];
		int len = ::recv(iter->second.get()->getfd(), temp,1, MSG_PEEK);
		while (len>0)
		{
			iter->second.get()->read_er();
			len = ::recv(iter->second.get()->getfd(), temp, 1, MSG_PEEK);
		}
	}
}

int main()
{
	srand((int)time(0));  // 产生随机种子
	for (int i = 0; i < 1; i++)
	{
		te();
		//threadpool_push(bind(&te));
	}
	cout << m_channels.size() << endl;
	//this_thread::sleep_for(std::chrono::seconds(10));
	cout << "按任意键进行下一步执行消息传输" << endl;
	string sssss;
	cin >> sssss;
	fa1();
	cout <<"当前连接数"<< m_channels.size() << endl;
	
	cin >> sssss;
	shou1();
	cout << "已收，再按一次任意键结束"<< endl;
	cin >> sssss;
	m_channels.clear();
	return 0;
}


