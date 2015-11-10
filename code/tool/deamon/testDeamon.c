#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    printf("pid = %d, ppid = %d\n", getpid(), getppid);

    sleep(10);

    return 0;
}
