#define _POSIX_C_SOURCE 200809L
#include "util.h"
#include <signal.h>
#include <string.h>

volatile sig_atomic_t clear_queue = 0;
int shell_running = 1;

#define QUEUE_SIZE 10
typedef struct {
    char* data[QUEUE_SIZE];
    int fp;
    int bp;
    size_t sz;
} queue;

void clear_item_queue(queue* q) {
    for(int it = q->fp; it != q->bp; it++) {
        free(q->data[it%QUEUE_SIZE]);
    }
    q->fp = q->bp = q->sz = 0;
}

void handle_sigint(int sig) {

    const char msg[] = "\n\n[ALERT] Emergency stop triggered, item queue cleared\n\n";
    write(STDOUT_FILENO, msg, sizeof(msg)-1);
    clear_queue = 1;
}

void add_item(queue* queue, char** args) {
    if(queue->sz == QUEUE_SIZE)  {
        printf("Queue full!\n");
        return;
    }

    queue->data[(queue->bp++)%QUEUE_SIZE] = strdup(args[0]);
    queue->sz++;
}

void list_items(queue* queue, char** args) {
    if(queue->sz == 0) {
        printf("Queue is empty.\n");
        return;
    }

    for(int it = queue->fp; it != queue->bp; it++) {
        printf("%s\n", queue->data[it%QUEUE_SIZE]);
    }
}

void quit(queue* queue, char** args) {
    shell_running = 0;
}

void date(char** args) {
    char *command[] = {"date", NULL};
    exec_process(command);
}

void ping(char** args) {
    char *command[] = {"ping", "-c", "4", args[0], NULL};
    exec_process(command);
}

void clear(char** args) {
    char* command[] = {"clear", NULL};
    exec_process(command);
}

typedef void (*if_ptr)(queue*, char**);
typedef void (*ef_ptr)(char**);

typedef struct {
    char *name;
    if_ptr function;
    size_t argc;
    char* usage;
} int_funct;

typedef struct {
    char *name;
    ef_ptr function;
    size_t argc;
    char* usage;
} ext_funct;

int main() {
    struct sigaction sa;
    sa.sa_handler = &handle_sigint;
    sigemptyset(&sa.sa_mask);
    
    sa.sa_flags = 0; 
    sigaction(SIGINT, &sa, NULL);

    queue item_queue = {.fp = 0, .bp = 0, .sz = 0};

    char command[1024];

    while(shell_running) {
        printf("belt-control$ ");

        if(clear_queue) {
            clear_item_queue(&item_queue);
            clear_queue = 0;
            continue;
        }

        int_funct int_commands[] = {
            {"add_item",    (if_ptr) add_item,      1, "add_item <name>"},
            {"list_items",  (if_ptr) list_items,    0, "list_items"},
            {"quit",        (if_ptr) quit,          0, "quit"},
        };

        ext_funct ext_commands[] = {
            {"date",        (ef_ptr) date,          0, "date"},
            {"ping",        (ef_ptr) ping,          1, "ping <url>"},
            {"clear",       (ef_ptr) clear,         0, "clear"}
        };

        if(fgets(command, sizeof(command), stdin) == NULL) {
            if(errno == EINTR && clear_queue) {
                clear_item_queue(&item_queue);
                clear_queue = 0;
                continue;
            }
            if(errno == EINTR) continue;

            break;
        }

        size_t clen = strlen(command);
        command[--clen] = '\0';
        if(!clen) continue;
        size_t argc = 0;

        for(int i = 0; i < clen; i++) {
            if(i && command[i] != ' ' && command[i-1] == ' ') argc++;
        }
        // parse args

        char** args = malloc(sizeof(char*)*argc);
        size_t carg = 0;

        for(int i = 0; i < clen; i++) {
            if(i && command[i] != ' ' && command[i-1] == '\0') args[carg++] = command+i;
            if(command[i] == ' ') command[i] = '\0';
        }

        for(int i = 0; i < sizeof(int_commands)/sizeof(int_commands[0]); i++) {
            if(!strcmp(command, int_commands[i].name)) {
                if(argc == int_commands[i].argc) int_commands[i].function(&item_queue, args);
                else {
                    printf("Invalid parameters for : %s\nUsage: %s\n", int_commands[i].name, int_commands[i].usage);
                }
                goto DONE;
            }
        }

        for(int i = 0; i < sizeof(ext_commands)/sizeof(ext_commands[0]); i++) {
            if(!strcmp(command, ext_commands[i].name)) {
                if(argc == ext_commands[i].argc) ext_commands[i].function(args);
                else {
                    printf("Invalid parameters for : %s\nUsage: %s\n", ext_commands[i].name, ext_commands[i].usage);
                }
                goto DONE;
            }
        }


        printf("Invalid command!\n");
DONE:   free(args);
    }

    clear_item_queue(&item_queue);
    return 0;

}