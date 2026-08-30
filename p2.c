#include "util.h"
#include <string.h>

#define MAX_DATA 512

typedef struct
{
    long type; // 5 for parent to child ; 10 for child to parent
    char data[MAX_DATA];
} mq_message;

int main(int argc, char *argv[]) {
    if(argc < 4) {
        printf("Invalid arguments passed to program\n");
        printf("Format: <pname> <n> <k> <r>\n");
        return -1;
    }

    long n = atoi(argv[1]);
    long k = atoi(argv[2]);
    long r = atoi(argv[3]);

    int mq_id = msgget(IPC_PRIVATE, 0666|IPC_CREAT);
    if(mq_id < 0) {
        perror("msgget");
        return -1;
    }

    pid_t ch = fork();
    if(ch < 0) {
        perror("forking child");
        return -1;
    }
    if(!ch) {

        char** commands[] = {
            (char*[]){"ps", "-axo", "pid,comm,%cpu,%mem", NULL},
            (char*[]){"awk", "NR > 1 {score = 3 * $(NF-1) + 2 * $NF; print $1, $2, $(NF-1), $(NF), score}", NULL},
            (char*[]){"column", "-t", NULL},
            (char*[]){"sort", "-k5,5rn", NULL},
            (char*[]){"head", "-n", argv[2], NULL}
        };

        size_t num_commands = sizeof(commands) / sizeof(commands[0]);

        while(1) {
            for(int i = 0; i < r; i++) {
                printf("PID      COMMAND          \%CPU  \%MEM  SCORE\n");
                exec_pipeline(commands, num_commands);
                printf("\n\n");
                sleep(n);

            }
            
            mq_message mqm;

            mqm.type = 10;
            strncpy(mqm.data, "R", MAX_DATA);

            if(msgsnd(mq_id, &mqm, strlen(mqm.data)+1, 0) < 0) {
                perror("mq send");
                return -1;
            }

            if (msgrcv(mq_id, &mqm, sizeof(mqm.data), 5, 0) < 0) {
                perror("mq receive");
                return -1;
            }

            int res = atoi(mqm.data);

            if(res == -2) {
                printf("Child process exiting..\n");
                mqm.type = 10;
                strncpy(mqm.data, "Q", MAX_DATA);

                if(msgsnd(mq_id, &mqm, strlen(mqm.data)+1, 0) < 0) {
                    perror("mq send");
                    exit(EXIT_FAILURE);
                    return -1;
                }
                exit(EXIT_SUCCESS);
            } else if(res == -1) {
                continue;
            } else {
                char proc_pid[20];
                snprintf(proc_pid, sizeof(proc_pid), "%d", res);

                char** info_commands[] = {
                    (char*[]){"ps", "-o", "ppid,comm,%cpu,%mem", "-p", proc_pid, NULL},
                    (char*[]){"awk", "NR == 1 { print $0, \"SCORE\" } NR > 1 {score = 3 * $(NF-1) + 2 * $NF; print $1, $2, $(NF-1), $(NF), score}", NULL},
                };
                
                exec_pipeline(info_commands, 2);

                kill(res, SIGKILL);
            }

        }

        exit(EXIT_FAILURE);
    }

    while(1) {
        mq_message mqm;
        if(msgrcv(mq_id, &mqm, sizeof(mqm.data), 10, 0) < 0){
            perror("mq receive");
            return -1;
        }

        if(mqm.data[0] == 'Q') {
            wait(NULL);
            if (msgctl(mq_id, IPC_RMID, NULL) == -1) {
                perror("mq remove");
                return -1;
            } 
            return 0;
        }
        printf("Enter your input [-2 (quit), -1 (continue), <pid>]: ");
        scanf("%511s", mqm.data);

        mqm.type = 5;
        if(msgsnd(mq_id, &mqm, strlen(mqm.data)+1, 0) < 0){
            perror("mq send");
            return -1;
        }
    }

    wait(NULL);
    if (msgctl(mq_id, IPC_RMID, NULL) == -1) {
        perror("mq remove");
        return -1;
    } 
    return 0;

}
