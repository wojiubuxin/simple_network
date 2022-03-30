#ifndef EPOLL_ER_H
#define EPOLL_ER_H
#include <sys/epoll.h>
#include <vector>
#include <map>
#include <errno.h>
#include <thread>
#include <unistd.h>

struct epoll_event;
namespace simple
{
	class socket_er;

	class epoll_er
	{
	public:
		epoll_er();
		~epoll_er();
		void start_epoll();
		void quit_ep();
		//void update_socket_er_Channel(socket_er* channel);
		void removeChannel(socket_er* channel);
		typedef std::map<int, std::shared_ptr<socket_er>>         ChannelMap;
		ChannelMap                              m_channels;
		void update(int operation, socket_er* channel);
	private:
		typedef std::vector<struct epoll_event> EventList;
		EventList                               m_events;

		std::unique_ptr<std::thread>ep_func;
		int      epollfd;
	
		bool run_epoll;
		
		void epoll_wait_thread_func();

		
		

	};

}
#endif