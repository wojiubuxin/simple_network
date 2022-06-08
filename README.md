#### 简洁的网络库，但愿能带给新手启发，倘若真能助人，我也会非常欣慰,也还有很长的路要走
#### 知识如海，航行期间是会被打击的怀疑人生，就算被吊打，吾辈也应当硬着头皮重新站起,破海前行
#### 路漫漫其修远兮，吾将上下而求索  
#### 这个库如其名简单(simple)，尽量用我已知的，踩过的，尽可能的，简单的书写出来，便于理解其思想
#### 之中可能会有些BUG，怕避免误导你们，不用死扣细节，理解其思路，做到心中了然即可
#### 若有大能者阅得此章，在下若是做的不好，请见谅，也亦可放心指出误区，在下也会欣然学习汲取，在此谢过

 <br />
 <br />   

### 整体代码是c++11编写的，在linux环境下运行，当然一些独立模块在windows下也能执行，例如日志，定时器，线程池
### 简单概括下当分开模块时的使用方法
### 日志库
1.直接头文件包含log.h拿来用就可以                <br />
2.例如                                         <br />
#include "log.h"                               <br />
using namespace simple;                        <br />
int main()<br />
{                                              <br />
  &emsp;logger("test","123","喝水","ewq");           <br />
  &emsp;loger("test","123","吃饭");                  <br />
  
  
  &emsp;//前面test是表示文件名的意思，后面都可以填写要记录的内容，支持变长参数 <br />
  &emsp;//logger,logger这是两个相等的宏，用哪个都可以，看个人习惯 <br />
  &emsp;//会在这个目录下生成文件,    /usr/local/zhu/logs/test_2022-05-31.log <br />
  &emsp;//这是linux下的生成路径,windows下也可以，需要微改下即可 <br />
  &emsp;//在这库(https://github.com/wojiubuxin/simple_understand )里windows直接用即可，我调了些细节,因为有些编译器会觉得某些函数或写法不安全，但大体跟linux是相似的<br />
  
  &emsp;//cat /usr/local/zhu/logs/test_2022-05-31.log 之后会显示以下内容 <br />
  &emsp;//2022-05-31 [14:46:01:765]|123|喝水|ewq <br />
  &emsp;//2022-05-31 [14:46:01:765]|123|吃饭 <br />
  
  &emsp;//头部包含时间年月日和毫秒级别的记录,应该日常够了,内容之间以 "|" 做分割 ,便于shell命令分割 <br />
  
  
  
  &emsp;return 0;<br />
}<br />
3.整体思路<br />
起了另一线程做为缓冲写入文件，总有两块缓冲区，一块用于当前写入缓冲，一块负责写入文件，特定情况下会进行交换缓冲区写入文件<br />
1.当缓冲区内没数据时，日志线程先挂起三秒<br />
2.当有数据写入缓冲区时，唤起日志线程进行加锁交换缓冲区<br />
3.日志线程交换完后，释放锁，再不断写入文件内，而主线程就一直把数据写在缓冲区里，从而两者不会影响堵塞到<br />
4.日志线程写完自己的文件后，再去尝试获取下锁，看下当前主线程的缓冲区里是否有数据，如果有就继续交换，如果没有就挂起三秒，不断反复<br />
5.跟主线程分离开来，就不会造成因为打日志时阻塞当前业务流程<br />
6.用到了智能指针，让它来帮我们释放，内存泄漏交给它处理即可<br />

### 定时器
1.直接头文件包含cb_time.h拿来用就可以                <br />
2.例如                                         <br />
#include "cb_time.h"                            <br />
#include "log.h"                               <br />
using namespace simple;                        <br />
void yacelog(int i)<br />
{<br />
	&emsp;loger("yace", "testxianchengchi",i);<br />
}<br />
void test1()<br />
{<br />
	&emsp;loger("yace", "test1");<br />
}<br />
class timer_ts<br />
{<br />
public:<br />
	&emsp;timer_ts() {};<br />
	&emsp;~timer_ts() {};<br />
	&emsp;void prints(int a)<br />
	&emsp;{
		&emsp;loger("yace", "prints",a);<br />
	&emsp;};<br />
};<br />

int main()<br />
{   <br />
  &emsp;timer_ts ads;<br />
	 &emsp;auto pr1 = std::bind(&yacelog, 123);<br />
	 &emsp;SetTimeCB(2, pr1);//前面参数是代表定时2秒，后面是压入的回调<br />
	 &emsp;SetTimeCB(0.5, test1);//这里是指0.5秒<br />
	 &emsp;auto pr2 = std::bind(&timer_ts::prints, &ads, 666);<br />
	 &emsp;SetTimeCB(1, pr2);//大概演示了三种设置定时器的方式<br />
	 &emsp;//KillTimeCB(test1);//这里是清除掉定时器,之前设置的时候传啥值,删的时候再传入就行<br />
	 &emsp;//KillTimeCB(pr1);<br />
  &emsp;return 0;<br />
  
  &emsp;//会在这个文件下/usr/local/zhu/logs/yace_2022-05-31.log生成以下内容<br />
  &emsp;//2022-05-31 [14:50:55:507]|test1<br />
  &emsp;//2022-05-31 [14:50:56:005]|prints|666<br />
  &emsp;//2022-05-31 [14:50:57:006]|testxianchengchi|123<br />
  &emsp;//间隔时间是0.5秒，1秒，大概会有1毫秒的延迟打印,整体效果对人感知来说，差距不大<br />
}<br />
3.整体思路<br />
1.SetTimeCB(设置定时器),KillTimeCB(清除定时器),采用了单例模式的方法<br />
2.支持毫秒级别的精度,也是起一个定时器线程，通过线程自带的睡眠机制来控制<br />
3.每压入一个，即会唤醒判断，用multimap来进行存储，即可最早需要触发的放在前列，也可放入重复的毫秒级时间戳<br />
4.全程也是交由智能指针和锁，让它帮我们管理和互斥定时队列<br />
5.也支持windows和linux平台<br />

### 线程池
1.直接头文件包含threadpool.h拿来用就可以                <br />
2.例如                                         <br />
#include "log.h"                               <br />
#include "threadpool.h"                         <br />
using namespace simple;                        <br />
void yacelog(int i)<br />
{<br />
	&emsp;loger("yace", "testxianchengchi",i);<br />
}<br />
void test1()<br />
{<br />
	&emsp;loger("yace", "test1");<br />
}<br />
class timer_ts<br />
{<br />
public:<br />
	&emsp;timer_ts() {};<br />
	&emsp;~timer_ts() {};<br />
	&emsp;void prints(int a)<br />
	&emsp;{
		&emsp;loger("yace", "prints",a);<br />
	&emsp;};<br />
};<br />

int main()<br />
{                                              <br />
	&emsp;timer_ts ads; <br />
	&emsp;threadpool_push(test1); <br />
	&emsp;threadpool_push(std::bind(yacelog,123)); <br />
	&emsp;threadpool_push(std::bind(&timer_ts::prints,&ads, 666)); <br />
	&emsp;return 0;<br />
	&emsp;//会在这个文件下/usr/local/zhu/logs/yace_2022-05-31.log生成以下内容<br />
	&emsp;//2022-05-31 [16:17:39:359]|test1<br />
    &emsp;//2022-05-31 [16:17:39:359]|testxianchengchi|123<br />
    &emsp;//2022-05-31 [16:17:39:359]|prints|666<br />

	
}<br />
3.整体思路<br />
1.起了四个工作线程挂起等待，当主业务压入时，先获取锁，压入后再释放锁，并只唤醒一个线程即可，防止惊群<br />
2.当苏醒的工作线程获取到锁后，再从队列里拿出一个任务，并释放锁，避免长时间占用，然后再执行<br />
3.执行完后，如果没任务了，再挂起即可，等待下次的唤醒，如此往复<br />



### 接下来是整体运用，内含epoll，socket
1.首先在建个目录,例如 mkdir /usr/local/zhu
2.再把相关文件放进去，接着参考CMakeLists.txt即可
3.然后在那个目录下执行  ./fucktest_server 和 ./fucktest_client即可
4.

## 入道者阅后即可简单初步了解整个知识体系，心中也能找到属于自己的方向，自己的学习思路
### 在下学识尚浅,倘若大能者阅得此章,愿洗耳恭听,指出在下的不足之处,烦请不吝赐教
![image](https://github.com/wojiubuxin/image/blob/master/zhongdu.jpeg)
