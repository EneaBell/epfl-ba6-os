#include <stdio.h>


int main(int argc, char* argv[]) {
    unsigned i;

    if (argc != 2) {
        printf("Usage: loop ch\n");
        return 1;
    }

    char c = argv[1][0];

    for(i=0; i<1000000000; ++i) {
        if(i % 10000000 == 0) {
            putchar(c);
            fflush(stdout);
        }
    }
    
    return 0;
}
