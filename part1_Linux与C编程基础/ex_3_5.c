#include <stdio.h>

int main() {
    int  i, j;
    long sum;
    for ( i = 0; i <= 10; i++ ) {
        int total = 1;
        for ( j = 0; j <= i; j++ ) {
            total *= j;
        }
        sum += total;
    }

    printf( "1!+2!+3!+...+10! = %ld\n", sum );

    return 0;
}
