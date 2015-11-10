#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#define FIND_NAME      "testDeamon"
#define RUN_NAME       "./testDeamon &"

int daemon(int nochdir, int noclose)  
{
    pid_t pid = 0; 
    
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
    
    //更改当前工作目录,将工作目录修改成根目录
    if (!nochdir)
        chdir("/"); 
    
    //关闭文件描述符，并重定向标准输入，输出合错误输出
    //将标准输入输出重定向到空设备
    if (!noclose) {
        int fd = 0;
        fd = open("/dev/null",O_RDWR,0);
        if (fd != -1) { 
            dup2(fd,STDIN_FILENO);
            dup2(fd,STDOUT_FILENO);
            dup2(fd,STDERR_FILENO);

            if (fd > 2)
                close(fd);
        }
    }
    
    //设置守护进程的文件权限创建掩码
    umask(0027);
    
    return 0; 
}  

/*
  check whether a process is running
  return: 0 -- not find
          1 -- find
         -1 -- error
*/
int checkProcessInRun(const char* p_processName)
{
    DIR *p_dir = NULL;
    struct dirent *pde = NULL;
    FILE *pf = NULL;
    char fileName[256] = {0};
    char buff[256] = {0};
    int len = 0;
    int i = 0;
    int ret = 0;

    p_dir = opendir("/proc");
    if (!p_dir)
        return -1;

    while ((pde = readdir(p_dir))) {
        if ((pde->d_name[0] < '0')
            || (pde->d_name[0] > '9'))
            continue;

        sprintf(fileName, "/proc/%s/status", pde->d_name);
        pf = fopen(fileName, "r");
        if (pf) {
            len = fread(buff, 1, 255, pf);
            fclose(pf);

            buff[len] = 0;

            for (i = 0; i < len; i++) {
                if ('\n' == buff[i]) {
                    buff[i] = 0;
                    break;
                }
            } 
        //printf("== (%s) ==\n", buff);
        
            len = i; 
            for (i = 0; i < len; i++) {
                if ((' ' == buff[i]) || ('\t' == buff[i]))
                    break;
            }

            for (; i < len; i++) {
                if ((' ' != buff[i]) && ('\t' != buff[i]))
                    break;
            }

            if (0 == strcmp(buff + i, p_processName)) {
                ret = 1;
                break;
            }
        }
    }

    closedir(p_dir);

    return ret;
}
 
int main(int argc, char** argv)
{
    char szFindName[256] = {0};
    char szRunName[256] = {0};
    char szCmd[256] = {0};
    char szBuf[4096] = {0};
    int fd = 0;    
    
    if (argc <= 2) {
        strcpy(szFindName, FIND_NAME);
        strcpy(szRunName, RUN_NAME);
    }
    else {
        strcpy(szFindName, argv[1]);
        strcpy(szRunName, argv[2]);
    }

    //initial
    if (daemon(1, 1))
        return -1;

    while (1) {
        //check whether the process is running
        if (checkProcessInRun(szFindName) != 1)
            system(szRunName);

        sleep(60);
    }

    return 0;
}
