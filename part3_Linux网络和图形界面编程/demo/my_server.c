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

#define SERV_PORT 4507  // 服务器端的端口
#define LISTENQ 12      // 连接请求队列的最大长度

#define INVALID_USERINFO 'n'  // 用户信息无效
#define VALID_USERINFO 'y'    // 用户信息有效

#define USERNAME 0  // 接收到的是用户名
#define PASSWORD 1  // 接收到的是密码

#define USERINFOSIZE 32
struct UserInfo {
    char username[ USERINFOSIZE ];
    char password[ USERINFOSIZE ];
};
struct UserInfo users[] = { { "linux", "unix" }, { "4507", "4508" }, { "clh", "clh" }, { "xl", "xl" }, { " ", " " } };

int FindName( const char* ptrNameString ) {

    if ( ptrNameString == NULL ) {
        printf( "In Function FindName, NULL ptrNameString\n" );
        return -2;
    }
    int i;
    for ( i = 0; users[ i ].username[ 0 ] != ' '; ++i ) {
        if ( strcmp( users[ i ].username, ptrNameString ) == 0 ) {
            return i;
        }
    }
    return -1;
}

void Send_data( int conn_fd, const char* MsgString ) {
    if ( send( conn_fd, MsgString, strlen( MsgString ), 0 ) < 0 ) {
        my_error( "send", __LINE__ );
    }
}

int main( int argc, char const* argv[] ) {
    int                sock_fd, conn_fd;
    int                optval;
    int                flag_recv = USERNAME;  // 标识接收到的是用户名还是密码
    int                ret;
    int                name_num;
    pid_t              pid;
    socklen_t          cli_len;
    struct sockaddr_in cli_addr, serv_addr;
    char               recv_buf[ 128 ];

    // 创建一个TCP套接字
    sock_fd = socket( AF_INET, SOCK_STREAM, 0 );

    if ( sock_fd < 0 ) {
        my_error( "socket", __LINE__ );
    }

    // 设置该套接字使之可以重新绑定端口
    optval = 1;
    if ( setsockopt( sock_fd, SOL_SOCKET, SO_REUSEADDR, ( void* )&optval, sizeof( int ) ) < 0 ) {
        my_error( "setsockopt", __LINE__ );
    }

    // 初始化服务器端地址结构
    memset( &serv_addr, 0, sizeof( struct sockaddr_in ) );
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_port        = htons( SERV_PORT );
    serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );

    // 将套接字绑定到本地端口
    if ( bind( sock_fd, ( struct sockaddr* )&serv_addr, sizeof( struct sockaddr_in ) ) < 0 ) {
        my_error( "sock_fd", __LINE__ );
    }

    // 将套接字转化为监听套接字
    if ( listen( sock_fd, LISTENQ ) < 0 ) {
        my_error( "listen", __LINE__ );
    }

    cli_len = sizeof( struct sockaddr_in );
    while ( 1 ) {
        conn_fd = accept( sock_fd, ( struct sockaddr* )&cli_addr, &cli_len );
        if ( conn_fd < 0 ) {
            my_error( "accept", __LINE__ );
        }

        printf( "accept a new client, ip:%s\n", inet_ntoa( cli_addr.sin_addr ) );
        if ( ( pid = fork() ) == 0 ) {
            if ( ( ret = recv( conn_fd, recv_buf, sizeof( recv_buf ), 0 ) ) < 0 ) {
                my_error( "recv", __LINE__ );
            }
            //
            recv_buf[ ret - 1 ] = '\0';

            if ( flag_recv == USERNAME ) {
                name_num = FindName( recv_buf );
                switch ( name_num ) {
                case -1:
                    Send_data( conn_fd, "N\n" );
                    break;
                case -2:
                    exit( 1 );

                default:
                    Send_data( conn_fd, "Y\n" );
                    flag_recv = PASSWORD;
                    break;
                }
            }
            else if ( flag_recv == PASSWORD ) {
                if ( strcmp( users[ name_num ].password, recv_buf ) == 0 ) {
                    Send_data( conn_fd, "Y\n" );
                    Send_data( conn_fd, "Welcom Login My TCP Server.\n" );
                    printf( "%s Login.\n", users[ name_num ].username );
                    break;
                }
                else {
                    Send_data( conn_fd, "N\n" );
                }
            }
            close( sock_fd );
            close( conn_fd );
            exit( 0 );
        }
        else {
            close( conn_fd );
        }
    }

    return 0;
}
