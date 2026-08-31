#ifndef OS_ASSIGNMENT_UTIL_H
#define OS_ASSIGNMENT_UTIL_H

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#define READ_END 0
#define WRITE_END 1

typedef int fd_t;

void swap(int *a, int* b);
int _gcd(int a, int b);
int gcd(int a, int b);

int read_payload(fd_t fd, void* dest, size_t size);
int write_payload(fd_t fd, void* src, size_t size);
int exec_pipeline(char*** commands, size_t num_commands);
int exec_process(char** command);

#endif