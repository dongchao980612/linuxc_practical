#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <error.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/types.h>

#include "my_recv.h"

#define INVALID_USERINFO 'n'  // 用户信息无效
#define VALID_USERINFO 'y'    // 用户信息有效

#define USERINFOSIZE 32

const char* CLIENT_USAGE = "Usage: [-a] [Server's IP address] [-p] [Server's Port]";

int get_user_info( char* buf, int len ) {
    /*  User input will be stored in buf  */
    //  Null pointer cannot transmit user input
    if ( buf == NULL ) {
        return -1;
    }
    //  Start saving user input data
    int  i = 0;
    char msg;
    while ( ( ( msg = getchar() ) != '\n' ) && ( msg != EOF ) && ( i < len - 2 ) ) {
        buf[ i++ ] = msg;
    }
    //  You have to leave two spaces here for the end sign
    buf[ i++ ] = '\n';
    buf[ i ]   = '\0';
    return 0;
}

void input_user_info( int conn_fd, const char* string ) {
    //  The username entered by the client is sent through conn_fd
    char input_buf[ USERINFOSIZE ];
    char recv_buf[ BUFSIZE ];
    int  flag_userInfo;

    do {
        //  Here MsgString is output on the client
        printf( "%s:", string );

        if ( get_user_info( input_buf, USERINFOSIZE ) < 0 ) {
            printf( "function GetUserInfo() return Error.\n" );
            exit( 1 );
        }

        //  Here it means that the user's input is normal
        if ( send( conn_fd, input_buf, strlen( input_buf ), 0 ) < 0 ) {
            my_error( "send", __LINE__ );
        }

        //  The server must judge this after sending it
        if ( my_recv( conn_fd, recv_buf, sizeof( recv_buf ) ) < 0 ) {
            printf( "Too much data entered.\n" );
            exit( 1 );
        }

        if ( recv_buf[ 0 ] == VALID_USERINFO ) {
            // The server indicates that the information entered is valid
            flag_userInfo = VALID_USERINFO;
        }
        else {
            printf( "The [%s] entered is invalid, please try again.\n", string );
            flag_userInfo = INVALID_USERINFO;
        }
    } while ( flag_userInfo == INVALID_USERINFO );
}

int main( int argc, char** argv ) {
    /*  Check the number of arguments  */
    if ( argc != 5 ) {
        printf( "%s\n", CLIENT_USAGE );
        exit( 1 );
    }

    /*  Initialize the server-side address structure  */
    struct sockaddr_in Server_addr;
    //  memset() will also clear the padding field __pad to zero
    memset( &Server_addr, 0, sizeof( struct sockaddr_in ) );
    Server_addr.sin_family = AF_INET;

    //  Get server-side ports and addresses from command line arguments
    int i;
    for ( i = 1; i < argc; ++i ) {  //  Skip executable command
        //  First find the server's IP address
        if ( strcmp( "-a", argv[ i ] ) == 0 ) {
            //  One position is [-a], then the next position is [Server's IP address]
            //  inet_pton():convert IPv4 addresses from text to binary form
            if ( inet_aton( argv[ i + 1 ], &Server_addr.sin_addr ) == 0 ) {
                printf( "Invalid Server IP Address\n" );
                exit( 1 );
            }
            continue;
        }
        //  Then find the server port number
        if ( strcmp( "-p", argv[ i ] ) == 0 ) {
            //  One position is [-p], then the next position is [Server's Port]
            int Server_port = atoi( argv[ i + 1 ] );
            //  Check the validity of the port number
            //  The port number ranges from 1 to 65535.
            if ( Server_port < 0 || 65535 < Server_port ) {
                printf( "Server's port number is invalid.\n" );
                exit( 1 );
            }
            //  host to internet short
            Server_addr.sin_port = htons( Server_port );
        }
    }

    //  Check if an argument is missing
    if ( Server_addr.sin_port == 0 || Server_addr.sin_addr.s_addr == 0 ) {
        printf( "%s\n", CLIENT_USAGE );
        exit( 1 );
    }

    /*  Create a TCP socket  */
    int conn_fd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( conn_fd < 0 ) {
        my_error( "socket", __LINE__ );
    }

    /*  Send a connection request to the server  */
    if ( connect( conn_fd, ( struct sockaddr* )&Server_addr, sizeof( struct sockaddr ) ) < 0 ) {
        my_error( "connect", __LINE__ );
    }

    /*  Enter username and password  */
    //  This step will determine whether the server
    //  is legal according to the user's input.
    input_user_info( conn_fd, "Username" );
    input_user_info( conn_fd, "Password" );

    //  Successful login and output welcome message
    char recv_buf[ BUFSIZE ];
    int  msglen;  //  The number of bytes read from the custom buffer
    if ( ( msglen = my_recv( conn_fd, recv_buf, sizeof( recv_buf ) ) ) < 0 ) {
        printf( "data is too long\n" );
        exit( 1 );
    }

    //  Output the message returned by the server after successful login
    for ( i = 0; i < msglen; ++i ) {
        printf( "%c", recv_buf[ i ] );
    }
    printf( "\n" );

    //  Close connection after successfully receiving welcome message
    close( conn_fd );
    return 0;
}
