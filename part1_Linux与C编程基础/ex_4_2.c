#include <stdio.h>

int myabs( int a );

int main( int argc, char const* argv[] ) {
    int x;
    scanf( "%d", &x );

    x = myabs( x );

    printf( "x = %d\n", x );

    return 0;
}

int myabs( int a ) {

    return a > 0 ? a : -a;
}