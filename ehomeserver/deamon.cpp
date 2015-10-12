/*

#include <stdio.h>   
#include <unistd.h>   
#include <sys/types.h>   
#include <sys/stat.h>   
#include <fcntl.h>  


//程序名字   
#define NAME "AlarmInterface -qws"   

//查找进程中程序名字   
#define NAME_FIND "AlarmInterface"   

//输出目录   
#define DIR_OUT_FILE "/rf/out"   

//要运行的程序   
#define RUN_NAME "DuiJiang &"   


//#define DIR_OUT_FILE "/rf/out"   
//#define NAME "gnome-keyring"   
//#define NAME_FIND "gnome"   
//#define DIR_OUT_FILE "/root/test/out"   

int daemon(int nochdir,int noclose)  
{ 
    pid_t pid; 
    
    //让init进程成为新产生进程的父进程
    pid = fork();
    
    //如果创建进程失败
    if (pid < 0) {
        perror("fork");
        return -1;
    }
    
    //父进程退出运行 
    if (pid != 0) {
        exit(0);
    }
    
    //创建新的会话
    pid = setsid();
    if (pid < -1) { 
        perror("set sid");
        return -1;
    }
    
    //更改当前工作目录,将工作目录修改成根目录
    if (!nochdir)
    {
        chdir("/"); 
    }
    
    //关闭文件描述符，并重定向标准输入，输出合错误输出
    //将标准输入输出重定向到空设备
    if (!noclose)
    {
        int fd;
        fd = open("/dev/null",O_RDWR,0);
        if (fd != -1)
        { 
            dup2(fd,STDIN_FILENO);
            dup2(fd,STDOUT_FILENO);
            dup2(fd,STDERR_FILENO);
            if (fd > 2)
            {
                close(fd);
            }
        }
    }
    
    //设置守护进程的文件权限创建掩码
    umask(0027);
    
    return 0; 
}  

//是否有匹配的字符，有则返回1，没有返回0   
//src:源字符串   
//dst：目标字符串   
//len:源字符串被比较的长度   
int match(char *src,char *dst,int len)  
{  
    int i = 0;  
    int j = 0;  
    int size_dst = 0;  
  
    //获得目标字符串的长度   
    size_dst = strlen(dst);  
    //如果目标字符串的长度大于len，返回失败   
    if (size_dst > len)  
    {  
        return 0;  
    }     
    //开始比较   
96.    for (i = 0;i < len;i++)  
97.    {  
98.        for (j = 0;j < size_dst;j++)  
99.        {  
100.            if (src[i + j] != dst[j])  
101.            {  
102.                break;  
103.            }  
104.        }  
105.        if (j == size_dst)  
106.        {  
107.            return 1;  
108.        }  
109.    }  
110.  
111.    return 0;  
112.}  
113.  
114.int main(int argc,char *argv[])  
115.{  
116.    int fd = 0;  
117.    char buf[100];  
118.  
119.    //开启守护进程   
120.    daemon(0,0);  
121.  
122.    while (1)  
123.    {  
124.        //打开日志   
125.        openlog(argv[0],LOG_CONS|LOG_PID,LOG_USER);  
126.          
127.        //查看程序是否运行   
128.        //新建输出文件   
129.        system("touch "DIR_OUT_FILE);  
130.        //获得程序ID   
131.        system("ps -w|grep "NAME_FIND" >> "DIR_OUT_FILE);  
132.        //打开输出文件   
133.        fd = open(DIR_OUT_FILE,O_CREAT|O_RDONLY,0777);  
134.        //清空缓存   
135.        memset(buf,0,100);  
136.        //读取全部   
137.        read(fd,buf,100);  
138.        //判断是否有程序文件运行   
139.        if (match(buf,NAME,90))  
140.        {  
141.            syslog(LOG_INFO,"jdh success!!!!!!!!!!");  
142.        }  
143.        else  
144.        {  
145.            syslog(LOG_INFO,"jdh fail!!!!!!!!!!");  
146.            //运行程序   
147.            system(RUN_NAME);  
148.        }  
149.  
150.        //休眠   
151.        sleep(5);  
152.        //删除输出文件   
153.        system("rm "DIR_OUT_FILE);  
154.          
155.        //休眠   
156.        sleep(55);  
157.    }  
158.  
159.    //关闭日志   
160.    closelog();  
161.  
162.    return 0;  
163.}  
*/