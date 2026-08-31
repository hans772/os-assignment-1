#include "util.h"

int main(int argc, char* argv[]) {
    if(argc < 3) {
        printf("Invalid arguments passed to program\n");
        printf("Format: <pname> <filepath> <column>\n");
        return -1;
    }

    char** commands[] = {
        (char*[]){"cut", "-d", " ", "-f", argv[2], argv[1], NULL},
        (char*[]){"sort", NULL},
        (char*[]){"uniq", "-c", NULL},
        (char*[]){"sort", "-rn", NULL},
        (char*[]){"head", "-5", NULL}
    };

    size_t num_commands = sizeof(commands) / sizeof(commands[0]);

    return exec_pipeline(commands, num_commands);
}