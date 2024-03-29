#include <stdio.h>

int main( int argc, char const* argv[] ) {
    int sum = 0, i = 1;
    do {
        sum += i;
        i++;
    } while ( i <= 100 );

    printf( "sum = %d\n", sum );
    
    return 0;
}
