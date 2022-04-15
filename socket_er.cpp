#include "socket_er.h"
#include "log.h"
#include "epoll_er.h"
#include "msg.h"
using namespace simple;

const int socket_er::kNoneEvent = 0;
const int socket_er::kReadEvent = POLLIN | POLLPRI;
const int socket_er::kWriteEvent = POLLOUT;

socket_er::socket_er() :events_(0), sockfd(-1),idleFd_(-1),revents_(0)
{
	m_loop = nullptr;
}

socket_er::~socket_er()
{
	int ret = ::close(sockfd);
	::close(idleFd_);
	if (ret< 0)
	{
		logger("error", "~socket_er", sockfd, ret);
	}
	logger("error", "~socket_er-2", sockfd, ret);
}

int socket_er::createNonblocksock()
{
	if (sockfd == -1)
	{
		//SOCK_NONBLOCK 设为非阻塞
		//SOCK_CLOEXEC fork调用创建子进程时在子进程中关闭该socket
		sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
		idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
		if (sockfd < 0 || idleFd_ < 0)
		{
			logger("error", "createNonblocksock", sockfd, idleFd_);
			return -1;
		}
		setReuseAddr();
		setReusePort();
		events_ |= kReadEvent;

		//监听本机器下的所有网卡
		std::string ip = "0.0.0.0";

		//结构体用666监听
		port = 666;

		//http用6666监听
		//port = 6666;

		memset(&addr_, 0, sizeof addr_);
		addr_.sin_family = AF_INET;
		addr_.sin_port = htobe16(port);
		::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
		bind_sock(&addr_);

		listen_sock();
	}
	return sockfd;
	
}


//客户端用的
int socket_er::createNonblocksock_client()
{
	if (sockfd == -1)
	{
		sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);

		std::string ip = "127.0.0.1";
		uint16_t port = 666;

		memset(&addr_, 0, sizeof addr_);
		addr_.sin_family = AF_INET;
		addr_.sin_port = htobe16(port);
		::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
		int flag = connect(&addr_);
		if (flag != 0 && errno!= EINPROGRESS)
		{
			loger("error", "创建客户端失败", sockfd, flag,errno);
			return sockfd;
		}
		loger("socket_er", "创建客户端成功", sockfd);

		setTcpNoDelay();
	}
	return sockfd;
	
}

void socket_er::bind_sock(const struct sockaddr_in* addr)
{
	int ret = ::bind(sockfd, sockaddr_cast(addr), static_cast<int>(sizeof(struct sockaddr_in)));
	if (ret < 0)
	{
		logger("error", "bind_sock", sockfd, ret);
	}
}

void socket_er::listen_sock()
{
	//第二个参数是全连接队列长度
	int ret = ::listen(sockfd, SOMAXCONN);
	if (ret < 0)
	{
		logger("error", "listen_sock", sockfd, ret);
	}
}

int socket_er::accept_sock(struct sockaddr_in* addr)
{
	socklen_t addrlen = static_cast<socklen_t>(sizeof *addr);
	int connfd = ::accept4(sockfd, sockaddr_cast(addr), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
	if (connfd < 0)
	{
		logger("error", "accept_sock", sockfd, connfd, errno);
	}
	return connfd;
}

void socket_er::setTcpNoDelay()
{
	//禁用Nagle算法
	//Nagle算法的作用是减少小包的数量，它是如何做到的呢(注意我是抄书的，如果不对欢迎指正)？
	//	什么是小包：小于 MSS(一个TCP段在网络上传输的最大尺寸) 的都可以定义为小包。
	//	如果前一个TCP段发送出去后，还没有收到对端的ACK包，那么接下来的发送数据会先累积起来不发。
	//	等到对端返回ACK，或者数据累积已快达到MSS，才会发送出去。
	int optval = 1;
	::setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY,&optval, static_cast<socklen_t>(sizeof optval));

}

//不用等TIME_WAIT状态结束，直接复用即可
void socket_er::setReuseAddr()
{
	int optval = 1;
	::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,&optval, static_cast<socklen_t>(sizeof optval));
}

void socket_er::setReusePort()
{
	int optval = 1;
	int ret = ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT,&optval, static_cast<socklen_t>(sizeof optval));
	if (ret < 0)
	{
		logger("error", "setReusePort", sockfd, ret);
	}
}

void socket_er::setKeepAlive()
{
	int optval = 1;
	::setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval));
}

void socket_er::setsockfd(int fd)
{
	if (sockfd == -1)
	{
		sockfd = fd;
	}
}

void socket_er::shutdownWrite()
{
	int ret = ::shutdown(sockfd, SHUT_WR);
	if (ret< 0)
	{
		logger("error", "shutdownWrite", sockfd, ret);
	}
}

int socket_er::connect(const struct sockaddr_in* addr)
{
	return ::connect(sockfd, sockaddr_cast(addr), static_cast<int>(sizeof(struct sockaddr_in)));
}

void socket_er::read_er()
{
	logger("socket_er", "read_er", sockfd);
	char temp[MAX_PACKAGE_SIZE];
	memset(temp, 0, sizeof(temp));
	//int  len=::read(sockfd, temp, sizeof(temp));

	//MSG_PEEK只读不删
	int  len = ::recv(sockfd, temp, sizeof(temp), MSG_PEEK);
	if (len > 0)
	{
		std::vector<char>temp_buffer;
		//先开辟一点临时空间用,防止频繁申请空间
		temp_buffer.reserve(50);

		temp_buffer.insert(temp_buffer.begin() + temp_buffer.size(), temp, temp + len);

		//不够一个包头大小
		if (temp_buffer.size()<(size_t)sizeof(msg_header))
		{
			//继续等待下次读
			logger("error", "read_er不够一个包头大小", sockfd, temp_buffer.size(), sizeof(msg_header));
			return;
		}

		msg_header header;
		memcpy(&header, &*temp_buffer.begin(), sizeof(header));
		if (header.packagesize <= 0 || header.packagesize > MAX_PACKAGE_SIZE)
		{
			//包头有错误，关闭连接
			logger("error", "read_er包头有错误，关闭连接", sockfd, header.packagesize);
			closeCallback_();
			return;
		}

		if (temp_buffer.size()< (size_t)header.packagesize + sizeof(msg_header))
		{
			//包体信息还不够
			logger("error", "read_er包体信息还不够", sockfd, temp_buffer.size(), (size_t)header.packagesize + sizeof(msg_header));
			return;
		}

		//够了就压入正式,不用全压,只获取当前请求信息的长度
		int len_flag = header.packagesize + sizeof(msg_header);
		buffer_read.insert(buffer_read.begin() + buffer_read.size(), temp, temp + len_flag);

		//这时再去去掉接收缓冲区里的数据，第四个参数设为0
		len = ::recv(sockfd, temp, len_flag, 0);

		messageCallback_();

	}
	else if (len == 0) //对面关掉连接了
	{
		closeCallback_();
	}
	else
	{
		logger("error", "read_er", sockfd, errno);
	}

}

void socket_er::write_er()
{
	logger("socket_er", "write_er", sockfd);
	//触发可写，之前没发完的继续发
	int max_len = buffer_write.size();
	if (max_len<=0)
	{
		return;
	}

	 char temp[MAX_PACKAGE_SIZE];
	 memset(temp, 0, sizeof(temp));

	 
	 if (max_len>MAX_PACKAGE_SIZE)
	 {
		 max_len = MAX_PACKAGE_SIZE;
	 }

	 memcpy(temp, &*buffer_write.begin(), max_len);

	 int len = ::write(sockfd, temp, max_len);
	 if (len > 0)
	 {
		 //写了一点就删掉
		 buffer_write.erase(buffer_write.begin(), buffer_write.begin() + len);

	 }

}

void socket_er::close_er()
{
	logger("socket_er", "close_er", sockfd);
	events_ = kNoneEvent;

	remove();
}

void socket_er::message_er()
{
	logger("socket_er", "message_er", sockfd, buffer_read.size());
	msg_header header;
	memcpy(&header, &*buffer_read.begin(), sizeof(header));

	//去除包头
	buffer_read.erase(buffer_read.begin(), buffer_read.begin() + sizeof(header));

	switch (header.msgid)
	{
	case MISSION_ASK:
		return mission_task();
	case MISSION_REPLY:
		return mission_task_rsp();
	case BUY_ASK:
		return buyitem();
	case BUY_REPLY:
		return buyitem_rsp();

	default:
		logger("error", "message_er-1");
		break;
	}

}

void socket_er::mission_task()
{
	msg_mission data;
	memcpy(&data, &*buffer_read.begin(), sizeof(data));
	//拿出后就去掉
	buffer_read.erase(buffer_read.begin(), buffer_read.begin() + sizeof(data));

	logger("mission", "你完成了任务", data.data,"干的漂亮");

	msg_mission_rsp data_rsp;
	data_rsp.code = 0;
	memcpy(data_rsp.msg, data.data, sizeof(data_rsp.msg));

	send(MISSION_REPLY, &data_rsp, sizeof(data_rsp));

}

void socket_er::mission_task_rsp()
{
	msg_mission_rsp data;
	memcpy(&data, &*buffer_read.begin(), sizeof(data));
	//拿出后就去掉
	buffer_read.erase(buffer_read.begin(), buffer_read.begin() + sizeof(data));

	logger("mission_client", "你完成了任务", data.code,data.msg);

}

void socket_er::buyitem()
{
	msg_buy data;
	memcpy(&data, &*buffer_read.begin(), sizeof(data));
	//拿出后就去掉
	buffer_read.erase(buffer_read.begin(), buffer_read.begin() + sizeof(data));


	logger("buy","用户", sockfd, "购买了", data.itemid,data.count, buffer_read.size());

	msg_buy_rsp data_rsp;
	data_rsp.code = 0;
	send(BUY_REPLY, &data_rsp, sizeof(data_rsp));
}

void socket_er::buyitem_rsp()
{
	msg_buy_rsp data;
	memcpy(&data, &*buffer_read.begin(), sizeof(data));
	//拿出后就去掉
	buffer_read.erase(buffer_read.begin(), buffer_read.begin() + sizeof(data));


	logger("buy_client", "用户", sockfd, "购买成功", data.code);

}

void socket_er::send(int msgid, void *buf, int size)
{
	msg_header send_data;
	send_data.msgid = msgid;
	send_data.packagesize = size;

	if (size+sizeof(msg_header)> MAX_PACKAGE_SIZE)
	{
		logger("error", "发送太长", sockfd, size);
		return;
	}

	char temp[MAX_PACKAGE_SIZE];
	memset(temp, 0, sizeof(temp));
	memcpy(temp, &send_data, sizeof(send_data));
	memcpy(temp+ sizeof(send_data), buf, size);

	int data_size = sizeof(send_data) + size;
	int len = ::write(sockfd, temp, data_size);

	
	if (len<data_size)
	{
		int remainslen = data_size - len;
		char leftover[MAX_PACKAGE_SIZE];
		memset(leftover, 0, sizeof(leftover));
		memcpy(leftover, temp + len, remainslen);

		buffer_write.insert(buffer_write.begin() + buffer_write.size(), leftover, leftover + remainslen);

		events_ |= kWriteEvent;
		//代表缓存区写满了,压入到epoll去，等空了再写
		if (m_loop)
		{
			m_loop->update(EPOLL_CTL_MOD, this);
		}
		
		
		
		logger("warning", "满了", sockfd, data_size, len);
	}

}

const struct sockaddr* socket_er::sockaddr_cast(const struct sockaddr_in* addr)
{
	return static_cast<const struct sockaddr*>((const void*)(addr));
}

struct sockaddr* socket_er::sockaddr_cast(struct sockaddr_in* addr)
{
	return static_cast<struct sockaddr*>((void*)(addr));
}

const struct sockaddr_in* socket_er::sockaddr_cast(const struct sockaddr* addr)
{
	return static_cast<const struct sockaddr_in*>((const void*)(addr));
}

struct sockaddr_in* socket_er::sockaddr_cast(struct sockaddr* addr)
{
	return static_cast<struct sockaddr_in*>((void*)(addr));
}

void socket_er::handleEvent()
{
	loger("socket_er", "handleEvent-1-", reventsToString().c_str());

	/*if ((revents_ & POLLHUP) && !(revents_ & POLLIN))
	{
		loger("socket_er", "handleEvent-2-", "handle_event() POLLHUP");
		if (closeCallback_) closeCallback_();
	}

	if (revents_ & (POLLERR | POLLNVAL))
	{
		loger("socket_er", "handleEvent-3-", "handle_event() POLLERR POLLNVAL");
		if (errorCallback_) errorCallback_();
	}*/

	if (revents_ & (POLLERR | POLLNVAL | POLLHUP))
	{
		if (closeCallback_)
		{
			loger("socket_er", "handleEvent-4-", "handle_event() POLLERR POLLNVAL POLLHUP");
			closeCallback_();
			return;
		}
		loger("socket_er", "handleEvent-5", "handle_event() POLLERR POLLNVAL POLLHUP");
	}

	if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
	{
		if (readCallback_) readCallback_();
	}
	if (revents_ & POLLOUT)
	{
		if (writeCallback_) writeCallback_();
	}

}

std::string socket_er::eventsToString()
{
	std::stringstream oss;
	oss << sockfd << ": ";
	if (events_ & POLLIN) // 读事件
		oss << "IN ";
	if (events_ & POLLPRI)//读事件，但表示紧急数据，例如tcp socket的带外数据
		oss << "PRI ";
	if (events_ & POLLOUT)//写事件
		oss << "OUT ";
	if (events_ & POLLHUP)//仅用于内核设置传出参数revents，表示设备被挂起，如果poll监听的fd是socket，表示这个socket并没有在网络上建立连接，比如说只调用了socket()函数，但是没有进行connect。
		oss << "HUP ";
	if (events_ & POLLRDHUP)
		//，Stream socket的一端关闭了连接（注意是stream socket，我们知道还有raw socket,dgram socket），或者是写端关闭了连接，如果要使用这个事件，必须定义_GNU_SOURCE 宏。这个事件可以用来判断链路是否发生异常（当然更通用的方法是使用心跳机制）。要使用这个事件，得这样包含头文件：
		//#define _GNU_SOURCE
		//#include <poll.h>
		oss << "RDHUP ";
	if (events_ & POLLERR)//仅用于内核设置传出参数revents，表示设备发生错误
		oss << "ERR ";
	if (events_ & POLLNVAL)//仅用于内核设置传出参数revents，表示非法请求文件描述符fd没有打开
		oss << "NVAL ";

	return oss.str();
}

std::string socket_er::reventsToString()
{
	std::stringstream oss;
	oss << sockfd << ": ";
	if (revents_ & POLLIN)
		oss << "IN ";
	if (revents_ & POLLPRI)
		oss << "PRI ";
	if (revents_ & POLLOUT)
		oss << "OUT ";
	if (revents_ & POLLHUP)
		oss << "HUP ";
	if (revents_ & POLLRDHUP)
		oss << "RDHUP ";
	if (revents_ & POLLERR)
		oss << "ERR ";
	if (revents_ & POLLNVAL)
		oss << "NVAL ";

	return oss.str();
}

void socket_er::remove()
{
	if (!isNoneEvent())
		return;

	m_loop->removeChannel(this);

}

void socket_er::Acceptor_handleRead()
{
	struct sockaddr_in peerAddr;
	memset(&peerAddr, 0, sizeof peerAddr);

	int connfd = accept_sock(&peerAddr);
	if (connfd >= 0)
	{
		//设置基础的参数值
		std::shared_ptr<socket_er> conn(new socket_er);
		conn->setloop(m_loop);
		conn->setsockfd(connfd);
		conn->setSockAddr(peerAddr);
		conn->setNoblock();
		conn->events_ |= kReadEvent;
		conn->setTcpNoDelay();
		conn->setKeepAlive();

		//设置读写关回调

		//以http格式解码
		if (port == 6666)
		{
			conn->setReadCallback(std::bind(&socket_er::read_er_http, conn.get()));
			conn->setMessageCallback(std::bind(&socket_er::message_er_http, conn.get()));
		}
		else
		{
			conn->setReadCallback(std::bind(&socket_er::read_er, conn.get()));
			conn->setWriteCallback(std::bind(&socket_er::write_er, conn.get()));
			conn->setMessageCallback(std::bind(&socket_er::message_er, conn.get()));
		}

		conn->setCloseCallback(std::bind(&socket_er::close_er, conn.get()));

		if (m_loop->m_channels.find(connfd) == m_loop->m_channels.end())
		{
			logger("socket_er", "Acceptor_handleRead-1-", connfd);
			m_loop->m_channels[connfd] = conn;

			//当我们接收到新的socket后要把它压入epoll去监听
			m_loop->update(EPOLL_CTL_ADD, conn.get());
		}
	

	}
	else
	{
		loger("error", "Acceptor_handleRead-4-");
		
		if (errno == EMFILE)
		{
			/*产生原因：
				进程打开的文件描述符达到上限，服务器accept时，返回EMFILE
				问题：
				listenfd一直处于监听状态，而此时服务器无法接受客户端的连接，所以会一直触发，产生busy loop
				处理方法：
				一开始打开一个空闲的文件描述符；
				当遇到上述情况时，先关闭这个文件描述符，此时将获得一个文件描述符的名额；
				再用accept接受客户端连接；
				立即关闭connectfd（优雅的与客户端断开连接）；
				重新打开一个文件描述符，以备再次出现上述情况。
				*/

			::close(idleFd_);
			idleFd_ = ::accept(sockfd, NULL, NULL);
			::close(idleFd_);
			idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
		}
	}
}

void socket_er::setNoblock()
{
	//设为非阻塞
	int flags = ::fcntl(sockfd, F_GETFL, 0);
	flags |= O_NONBLOCK;
	int ret = ::fcntl(sockfd, F_SETFL, flags);
	flags = ::fcntl(sockfd, F_GETFD, 0);
	flags |= FD_CLOEXEC;
	ret = ::fcntl(sockfd, F_SETFD, flags);
	(void)ret;
}


void socket_er::split(const char* temp, const char* delim, std::vector<std::string>& v)
{
	std::string buf(temp);
	int delimiterlength = strlen(delim);
	size_t pos = 0;
	std::string substr;
	while (true)
	{
		pos = buf.find(delim);
		if (pos != std::string::npos) //一个结束标记值
		{
			substr = buf.substr(0, pos);
			if (!substr.empty())
			{
				v.push_back(substr);
			}

			buf = buf.substr(pos + delimiterlength);
		}
		else
		{
			if (!buf.empty())
				v.push_back(buf);
			break;
		}
	}

}

//先判断下结束尾巴有吗
bool socket_er::ishavefenge(const char* temp, const char* delim)
{
	std::string buf(temp);
	size_t pos = 0;
	pos = buf.find(delim);
	if (pos != std::string::npos) //一个结束标记值
	{
		//存在
		return true;
	}


	return false;
}


void socket_er::read_er_http()
{
	logger("socket_er", "read_er_http", sockfd);
	char temp[MAX_PACKAGE_SIZE];
	memset(temp, 0, sizeof(temp));
	
	//MSG_PEEK只读不删
	int  len = ::recv(sockfd, temp, sizeof(temp), MSG_PEEK);
	if (len > 0)
	{
		
		//不够一个包头大小
		//因为一个http包头的数据至少\r\n\r\n，所以大于4个字符
		//小于等于4个字符，说明数据未收完，退出，等待网络底层接着收取
		if (len <= 4)
		{
			//继续等待下次读
			logger("error", "read_er_http不够一个包头大小", sockfd, len, temp);
			return;
		}

		//我们收到的GET请求数据包一般格式如下：
		//GET /simple/network?fuckyou=123&qfs=123 HTTP/1.1\r\n
		//User - Agent: curl / 7.29.0\r\n
		//Host : 127.0.0.1 : 666\r\n
		//Accept : */*\r\n
		//\r\n

		//检查是否以\r\n\r\n结束，如果不是说明包头不完整，退出
		if (ishavefenge(temp,"\r\n\r\n") == false)
		{
			logger("error", "read_er_http没有以rnrn结束", sockfd, temp);
			closeCallback_();
			return;
		}

		std::string tempff(temp);

		int shanchu_len = tempff.find("\r\n\r\n") + 4 ;

		//inbuf才是我们需要的
		std::string inbuf = tempff.substr(0, shanchu_len);

		
		//以\r\n分割每一行
		std::vector<std::string> lines;
		split(inbuf.c_str(),"\r\n" , lines);
		if (lines.size() < 1 || lines[0].empty())
		{
			logger("error", "read_er_http分割不对", sockfd, temp);
			closeCallback_();
			return;
		}

		//当前函数为了简单处理只支持get方法操作
		//至于其它post或是Keep-Alive之类的处理等等，读者喜欢造轮子可自行优化
		//chunk中至少有三个字符串：GET+url+HTTP版本号
		std::vector<std::string> chunk;
		split(lines[0].c_str()," ",chunk);
		if (chunk.size() < 3)
		{
			logger("error", "read_er_http没有三个字符串", sockfd, temp);
			closeCallback_();
			return;
		}
		
		//剩下的大概是这些数据了     /simple/network?asd=123&qwe=456
		buffer_read_http = chunk[1];
		//这时再去去掉接收缓冲区里的数据，第四个参数设为0
		len = ::recv(sockfd, temp, shanchu_len, 0);

		messageCallback_();

	}
	else if (len == 0) //对面关掉连接了
	{
		logger("error", "read_er_http-1", sockfd, errno);
		closeCallback_();
	}
	else
	{
		logger("error", "read_er_http", sockfd, errno);
	}

}


void socket_er::message_er_http()
{
	logger("socket_er", "message_er_http", sockfd, buffer_read_http);
	//开始剥离前面的问号?
	//例如/simple/network?itemid=123&act=buy&count=10
	//    /simple/network?qwe=456&act=mission_task
	std::vector<std::string>parms;
	split(buffer_read_http.c_str(), "?", parms);
	
	//那么parms[0]=  /simple/network
	//parms[1]=  itemid=123&act=buy&count=10

	if (parms[0] == "/simple/network")
	{
		//那么parms[1]准备二次分割,以&分割
		//itemid=123&act=buy&count=10或qwe=456&act=mission_task
		std::vector<std::string>parms_two;
		split(parms[1].c_str(), "&", parms_two);

		//准备第三次切割，以=分割
		//parms_two[0]=   itemid=123
		//parms_two[1]=   act=buy
		//parms_two[2]=   count=10
		//或是
		//parms_two[0]=   qwe=123
		//parms_two[1]=   act=mission_task
		std::string act = "";
		std::map<std::string, std::string> parms_three;
		for (auto iter = parms_two.begin(); iter != parms_two.end();iter++)
		{
			std::vector<std::string>parms_four;
			split(iter->c_str(), "=", parms_four);

			if (parms_four[0]=="act")
			{
				//触发的哪个函数
				act = parms_four[1];
			}
			else
			{
				//一些基础参数
				parms_three[parms_four[0]] = parms_four[1];
			}

		}

		//分割后开始做逻辑处理
		if (act=="buy")
		{
			loger("buy_http", "购买成功", "道具ID为", parms_three["itemid"], "数量为", parms_three["count"]);

		}

		if (act == "mission_task")
		{
			loger("mission_http", "任务完成","任务名为", parms_three["name"]);
		}

		//统一返回个200
		//手动组装http协议应答包
		/*
		HTTP/1.1 200 OK\r\n
		Content-Type: text/html\r\n
		Content-Length: 4864\r\n
		\r\n\
		{"code": 0, "msg": ok}
		*/
		std::string ret_msg = "{\"ret\": 0, \"msg\": ok}" + buffer_read_http;
		std::ostringstream os;
		os << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: "
			<< ret_msg.size() << "\r\n\r\n"
			<< ret_msg;
		
		std::string succ(os.str());
		int len = ::write(sockfd, succ.c_str(), succ.size());
		loger("socket_er", "写入结果", "len", len, succ);
	}
	else
	{
		logger("error", "message_er_http数据异常", sockfd, buffer_read_http);
	}

	//短链接处理处理完直接关掉
	closeCallback_();

}
