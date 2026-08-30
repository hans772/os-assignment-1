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