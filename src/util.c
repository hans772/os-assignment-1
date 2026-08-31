#include "util.h"

void swap(int *a, int* b) {
    int t = *b;
    *b = *a;
    *a = t;
}
int _gcd(int a, int b) {
    if(!(a%b)) return b;
    else return _gcd(b, a%b);
}

int gcd(int a, int b) {
    if(a < b) swap(&a, &b);
    return _gcd(a, b);
}

int read_payload(fd_t fd, void* dest, size_t size) {
    size_t rem = size;
    char *p = (char*) dest;
    ssize_t psize;

    while(rem > 0) {
        psize = read(fd, p, rem);

        if(!psize) break;
        if(psize < 0) {
            perror("connection closed or read failed!");
            return psize;
        }

        p += psize;
        rem -= psize;
    }

    if(!rem) return size;
    else return 0;
}

int write_payload(fd_t fd, void* src, size_t size) {
    size_t rem = size;
    const char *p = (const char*) src;
    ssize_t psize;

    while(rem > 0) {
        psize = write(fd, p, rem);

        if(!psize) break;
        if(psize < 0) {
            if(errno == EINTR) continue;
            perror("connection closed or write failed!");
            return psize;
        }

        p += psize;
        rem -= psize;
    }

    if(!rem) return size;
    else return 0;
}

int exec_process(char** command) {
    
    pid_t ch = fork();

    if(ch < 0) {
        perror("forking child");
        return -1;
    }

    if(!ch) {
        execvp(command[0], command);
        exit(EXIT_SUCCESS);
        return -1;
    }

    wait(NULL);
    return 0;
}

int exec_pipeline(char*** commands, size_t num_commands) {
    fd_t datapipe[2];
    fd_t p_inp;

    pid_t children[num_commands];

    
    for(int c = 0; c < num_commands; c++) {

        if(c < num_commands-1) {
            // new pipe only needs to be created if there is a next command to run
            if(pipe(datapipe) < 0) {
                perror("Error creating pipe!");
                return -1;
            };
        }

        pid_t ch = fork();
        if(ch < 0) {
            perror("Error creating child!");
            return -1;
        }

        if(!ch) {
            // inside child
            // if first process, we dont reroute stdin
            if(c) {
                dup2(p_inp, STDIN_FILENO);
                // the input is now handled by stdin, well close the fd to prevent issues
                close(p_inp);
            }
            // if last process we dont reroute stdout
            if(c < num_commands-1) {
                // child doesnt use read end of pipe
                close(datapipe[READ_END]);
                dup2(datapipe[WRITE_END], STDOUT_FILENO);
                //stdout handles write end, we can close pipe fd
                close(datapipe[WRITE_END]);
            }

            execvp(commands[c][0], commands[c]);
            _exit(EXIT_SUCCESS);
        }

        children[c] = ch;
        if(c) close(p_inp);
        if(c<num_commands-1) {
            close(datapipe[WRITE_END]);
            p_inp = datapipe[READ_END];
        }
    }


    // wait for pipelone to finish

    int status = 0;
    int last_status = 0;

    for (size_t c = 0; c < num_commands; c++) {
        waitpid(children[c], &status, 0);
        if (c == num_commands - 1) {
            if (WIFEXITED(status)) {
                last_status = WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                last_status = 128 + WTERMSIG(status);
            }
        }
    }

    return last_status;

}
