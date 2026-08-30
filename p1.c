#include "util.h"

typedef struct {
    int x;
    int y;
} p2c_payload;

typedef int c2p_payload;


int main() {
 
    pid_t ch;
    fd_t pipe_1[2];
    fd_t pipe_2[2];

    if(pipe(pipe_1) < 0 || pipe(pipe_2) < 0) {
        perror("Could not create pipe!");
        return -1;
    };

    if((ch = fork()) < 0) {
        perror("Could not fork process!");
        return -1;
    };

    if(!ch) {
        // child process
        close(pipe_1[WRITE_END]); // child isnt writing to the first pipe
        close(pipe_2[READ_END]); // child wont be reading from second pipe
        p2c_payload payload;
        int res;
        while((res = read_payload(pipe_1[READ_END], &payload, sizeof(p2c_payload))) > 0) {
            c2p_payload g = gcd(payload.x, payload.y);
            printf("Received %d & %d (GCD : %d)\n", payload.x, payload.y, g);
            usleep((time(NULL)%g)*1000);
            write_payload(pipe_2[WRITE_END], &g, sizeof(c2p_payload));
        }
        if(!res) printf("Parent closed connection.\n");
        close(pipe_1[READ_END]);
        close(pipe_2[WRITE_END]);
        exit(0);

    } else {
        // parent process

        close(pipe_1[READ_END]); // parent is not reading from first pipe
        close(pipe_2[WRITE_END]); // parent is not writing to second pipe

        int n;
        scanf("%d", &n);

        if(n%2) {
            kill(ch, SIGTERM);
            wait(NULL);
            perror("Invalid input!");
            return -1;
        }

        int* arr = malloc(sizeof(int) * n);
        for(int i = 0; i < n; i++) {
            scanf("%d", arr+i);
        }
        for(int i = 0; i < n; i++) {
            swap(arr+i, arr+(rand()%n));
        }
        p2c_payload payload;
        c2p_payload g;
        
        for(int i = 0; i < n; i+=2) {
            payload.x = arr[i];
            payload.y = arr[i+1];
            write_payload(pipe_1[WRITE_END], &payload, sizeof(p2c_payload));
            read_payload(pipe_2[READ_END], &g, sizeof(c2p_payload));
            printf("Received g: %d\n", g);
            usleep(g*1000);
        }

        close(pipe_1[WRITE_END]); 
        close(pipe_2[READ_END]); 
        wait(NULL);
    }

    printf("Operation Completed.\n");
}