#### 简洁的网络库，但愿能带给新手启发，倘若真能助人，我也会非常欣慰,也还有很长的路要走
#### 知识如海，航行期间是会被打击的怀疑人生，就算被吊打，吾辈也应当硬着头皮重新站起,破海前行
#### 路漫漫其修远兮，吾将上下而求索  


  
    
     
### 简单概括下使用方法
### 日志库
1.直接头文件包含log.h拿来用就可以                <br />
2.例如                                         <br />
#include "log.h"                               <br />
int main()<br />
{                                              <br />
    >logger("test","123","喝水","ewq");           <br />
    &emsp loger("test","123","吃饭");                  <br />
  
  
  //前面test是表示文件名的意思，后面都可以填写要记录的内容
  //logger,logger这是两个相等的宏，用哪个都可以，看个人习惯
  //会在这个目录下生成文件,    /usr/local/zhu/logs/test_2022-05-31.log
  //cat /usr/local/zhu/logs/test_2022-05-31.log 之后会显示以下内容
  //2022-05-31 [14:46:01:765]|123|喝水|ewq
  //2022-05-31 [14:46:01:765]|123|吃饭
  
  //头部包含时间年月日和毫秒级别的记录,应该日常够了,内容之间以 "|" 做分割 ,便于shell命令分割
  
  
  
  return 0;
}

 
## 入道者阅后即可简单初步了解整个知识体系，心中也能找到属于自己的方向，自己的学习思路
### 在下学识尚浅,倘若大能者阅得此章,愿洗耳恭听,指出在下的不足之处,烦请不吝赐教
![image](https://github.com/wojiubuxin/image/blob/master/zhongdu.jpeg)
