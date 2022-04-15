#ifndef SOCKET_ER_H
#define SOCKET_ER_H
#include <mutex>
#include <thread>
#include <condition_variable>
#include <deque>
#include <vector>
#include <map>
#include <chrono>
#include <time.h>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <endian.h>
#include <fcntl.h>
#include <thread>
namespace simple
{
	class epoll_er;
	class socket_er
	{
	public:
		typedef std::function<void()> EventCallback;

		socket_er();
		~socket_er();
		int createNonblocksock();
		int createNonblocksock_client();
		void set_revents(int revt) { revents_ = revt; }//系统定的
		void set_index(int idx) { index = idx; }			  //用户自己定义
		void set_events(int evx) { events_ = evx; }	  // 用户自己定义

		int getfd() const { return sockfd; }
		int get_revents() const { return revents_; }
		int get_index() const { return index; }
		int get_events() const { return events_; }


		void handleEvent();
		std::string eventsToString();
		std::string reventsToString();
		void setReadCallback(EventCallback cb)
		{
			readCallback_ = std::move(cb);
		}
		void setWriteCallback(EventCallback cb)
		{
			writeCallback_ = std::move(cb);
		}
		void setCloseCallback(EventCallback cb)
		{
			closeCallback_ = std::move(cb);
		}
		void setErrorCallback(EventCallback cb)
		{
			errorCallback_ = std::move(cb);
		}
		void setMessageCallback(EventCallback cb)
		{
			messageCallback_ = std::move(cb);
		}

		void setloop(epoll_er *loop)
		{
			m_loop = loop;
		}

		bool isNoneEvent() const { return events_ == kNoneEvent; }
		void remove();
		void listen_sock();
		int events_;
		static const int kNoneEvent;
		static const int kReadEvent;
		static const int kWriteEvent;
		//监听专用的
		void Acceptor_handleRead();
		void send(int msgid, void *buf, int size);
		void read_er();
		void write_er();
		void close_er();
		void message_er();

		//http格式
		void read_er_http();
		void message_er_http();

		void split(const char* temp, const char* delim, std::vector<std::string>& v);
		bool ishavefenge(const char* temp, const char* delim);
	private:
		epoll_er* m_loop;
		uint16_t port;
		void bind_sock(const struct sockaddr_in* addr);
		
		int accept_sock(struct sockaddr_in* addr);
		void setTcpNoDelay();
		void setReuseAddr();
		void setReusePort();
		void setKeepAlive();
		void setsockfd(int fd);
		void shutdownWrite();
		void setNoblock();

		int connect(const struct sockaddr_in* addr);
		
		

		struct sockaddr_in addr_;
		void setSockAddr(const struct sockaddr_in& addr) { addr_ = addr; }

		const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);

		struct sockaddr* sockaddr_cast(struct sockaddr_in* addr);

		const struct sockaddr_in* sockaddr_cast(const struct sockaddr* addr);

		struct sockaddr_in* sockaddr_cast(struct sockaddr* addr);

		int sockfd;
		//用于腾空名额用
		int idleFd_;
		
		int revents_;
		int index;
		
		EventCallback readCallback_;
		EventCallback writeCallback_;
		EventCallback closeCallback_;
		EventCallback errorCallback_;
		EventCallback messageCallback_;
		

		std::vector<char> buffer_read;
		std::vector<char> buffer_write;
		size_t readerIndex_;
		size_t writerIndex_;

		std::string buffer_read_http;
		
		//业务专用
		void mission_task();
		void mission_task_rsp();

		void buyitem();
		void buyitem_rsp();
		

	};

}
#endif