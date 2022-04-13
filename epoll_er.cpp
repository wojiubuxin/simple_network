#include "epoll_er.h"
#include "log.h"
#include "threadpool.h"
#include "socket_er.h"
using namespace simple;

epoll_er::epoll_er():m_events(100),run_epoll(true)
{
	epollfd = ::epoll_create1(EPOLL_CLOEXEC);

	if (epollfd < 0)
	{
		logger("error", "epoll_er", epollfd);
	}
	

}

epoll_er::~epoll_er()
{
	quit_ep();
	ep_func->join();
	m_channels.clear();

	::close(epollfd);
	logger("error", "~epoll_er");
}

void epoll_er::epoll_wait_thread_func()
{
	
	while (run_epoll)
	{	
		int numEvents = ::epoll_wait(epollfd,&*m_events.begin(),static_cast<int>(m_events.size()), 5000);//5秒没事件也返回

		if (numEvents > 0)
		{
			for (int i = 0; i < numEvents; i++)
			{
				socket_er* channel = static_cast<socket_er*>(m_events[i].data.ptr);
				int fd = channel->getfd();
				ChannelMap::const_iterator it = m_channels.find(fd);

				//有不存在的fd在操作信息
				if (it == m_channels.end() || it->second.get() != channel)
				{
					logger("error", "epoll_wait_thread_func-1-", fd);
					break;
				}

				channel->set_revents(m_events[i].events);

				channel->handleEvent();

			}

			if (static_cast<size_t>(numEvents) == m_events.size())
			{
				m_events.resize(m_events.size() * 2);
				logger("epoll", "epoll_wait_thread_func-2-", numEvents);
			}
		}
		else if (numEvents == 0)
		{
			//logger("error", "epoll_wait_thread_func-3-", "nothing happened");
		}
		else
		{
			logger("error", "epoll_wait_thread_func-4-", errno);
		}

	}

}

void epoll_er::start_epoll()
{
	auto test_ptr = std::bind(&epoll_er::epoll_wait_thread_func, this);
	std::unique_ptr<std::thread> temp(new std::thread(test_ptr));
	ep_func = std::move(temp);
}

void epoll_er::quit_ep()
{
	run_epoll = false;
}


void epoll_er::update(int operation, socket_er* channel)
{
	struct epoll_event event;
	memset(&event, 0, sizeof event);
	event.events = channel->get_events();	
	event.data.ptr = channel;
	int fd = channel->getfd();
	logger("epoll", "update-1-", operation, fd, channel->eventsToString().c_str());
	if (::epoll_ctl(epollfd, operation, fd, &event) < 0)
	{
		if (operation == EPOLL_CTL_DEL)
		{
			logger("epoll", "update-2-", fd, channel->eventsToString().c_str());
		}
		else
		{
			logger("epoll", "update-3-", fd, channel->eventsToString().c_str());
		}
	}

}


void epoll_er::removeChannel(socket_er* channel)
{
	int fd = channel->getfd();
	if (m_channels.find(fd) == m_channels.end() || m_channels[fd].get() != channel)
		return;


	if ( !channel->isNoneEvent())
	{
		return;
	}

	
	update(EPOLL_CTL_DEL, channel);

	size_t n = m_channels.erase(fd);
	if (n != 1)
		return;


}
