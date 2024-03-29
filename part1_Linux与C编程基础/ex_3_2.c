#include <stdio.h>

int main( int argc, char const* argv[] ) {
    int sum = 0, i = 1;
    while ( i <= 100 ) {
        sum += i;
        i++;
    }

    printf( "sum = %d\n", sum );
    
    return 0;
}
