#### 简洁的网络库，但愿能带给新手启发，倘若真能助人，我也会非常欣慰,也还有很长的路要走
#### 知识如海，航行期间是会被打击的怀疑人生，就算被吊打，吾辈也应当硬着头皮重新站起,破海前行
#### 路漫漫其修远兮，吾将上下而求索  
#### 这个库如其名简单(simple)，尽量用我已知的，踩过的，尽可能的，简单的书写出来，便于理解其思想
#### 之中可能会有些BUG，怕避免误导你们，不用死扣细节，理解其思路，做到心中了然即可
#### 若有大能者阅得此章，在下若是做的不好，请见谅，也亦可放心指出误区，在下也会欣然学习汲取，在此谢过

 <br />
 <br />   
     
### 简单概括下当分开模块时的使用方法
### 日志库
1.直接头文件包含log.h拿来用就可以                <br />
2.例如                                         <br />
#include "log.h"                               <br />
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
跟主线程分离开来，就不会造成因为打日志时阻塞当前业务流程<br />
5.用到了智能指针，让它来帮我们释放，内存泄漏交给系统处理<br />
 
## 入道者阅后即可简单初步了解整个知识体系，心中也能找到属于自己的方向，自己的学习思路
### 在下学识尚浅,倘若大能者阅得此章,愿洗耳恭听,指出在下的不足之处,烦请不吝赐教
![image](https://github.com/wojiubuxin/image/blob/master/zhongdu.jpeg)
