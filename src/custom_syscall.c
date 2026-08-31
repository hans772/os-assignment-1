#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#define __NR_setnice_logged 472
int setnice_logged(int nice_val)
{
    return syscall(__NR_setnice_logged, nice_val);
}
int main()
{
    int ret = setnice_logged(-2);
    if (ret == 0)
        printf("Nice value successfully changed.\n");
    else
        perror("setnice_logged failed");
    return 0;
}