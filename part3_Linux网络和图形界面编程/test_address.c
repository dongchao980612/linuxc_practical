#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int main( int argc, char const* argv[] ) {
    char           buffer[ 32 ];
    int            ret     = 0;
    int            host    = 0;
    int            network = 0;
    unsigned int   address = 0;
    struct in_addr in;

    in.s_addr = 0;

    /*输入一个以"."为分割的字符串形式的IP地址*/
    // printf( "please input you ip address:" );
    // fgets( buffer, 31, stdin );
    // buffer[ 31 ] = '\0';

    /*test ip address*/
    // strcpy( buffer, "172.17.242.131" );
    strcpy( buffer, "255.255.255.255" );

    /*示例使用 inet_aton() 函数*/
    if ( ( ret = inet_aton( buffer, &in ) == 0 ) ) {
        printf( "inet_aton:\tinvaild address\n" );
    }
    else {
        printf( "inet_aton:\t0x%x\n", in.s_addr );
    }

    /*示例使用 inet_addr() 函数*/
    if ( ( address = inet_addr( buffer )) == INADDR_NONE ) {
        printf( "inet_addr:\tinvaild address\n" );
    }
    else {
        printf( "inet_addr:\t0x%lx\n", address );
    }

    /*示例使用 inet_network() 函数*/
    if ( ( address = inet_network( buffer ) )== -1 ) {
        printf( "inet_network:\tinvaild address\n" );
    }
    else {
        printf( "inet_network:\t0x%lx\n", address );
    }

    /*示例使用 inet_ntoa() 函数*/
    if ( inet_ntoa( in ) == NULL ) {
        printf( "inet_ntoa:\tinvaild address\n" );
    }
    else {
        printf( "inet_ntoa:\t%s\n", inet_ntoa( in ) );
    }

    /*示例使用 inet_lnaof() 与 inet_lnaof() 函数*/
    host    = inet_lnaof( in );
    network = inet_netof( in );
    printf( "inet_lnaof:\t0x%x\n", host );
    printf( "inet_netof:\t0x%x\n", network );

    in = inet_makeaddr( network, host );
    printf( "inet_makeaddr:\t0x%x\n", in.s_addr );

    return 0;
}
