/*端口扫描程序，只支持TCP端口*/
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <errno.h>
#include <error.h>
#include <unistd.h>

#include <pthread.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

/*定义一个端口区间信息*/
typedef struct port_segment_t {
    struct in_addr     dest_ip;   //目标ip
    unsigned short int min_port;  //起始端口
    unsigned short int max_port;  //最大端口

} port_segment;

/// @brief 错误处理函数
/// @param err_string   错误信息字符串
/// @param line         错误信息所在行
void my_err_handler( const char* err_string, int line ) {
    fprintf( stderr, "line : %d ", line );
    perror( err_string );
    exit( 1 );
}

/// @brief 扫面某一ip地址的某个端口的函数
/// @param serv_addr
/// @return -1 出错
/// @return 0 目标端口未打开
/// @return 1 目标端口已经打开
int do_scan( struct sockaddr_in serv_addr ) {
    int conn_fd;
    int ret;

    /*创建一个TCP套接字*/
    conn_fd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( conn_fd < 0 ) {
        my_err_handler( "socket", __LINE__ );
    }
    /*向服务器发出连接请求*/
    if ( ( ret = connect( conn_fd, ( struct sockaddr* )&serv_addr, sizeof( struct sockaddr ) ) ) < 0 ) {
        my_err_handler( "connect", __LINE__ );
        if ( errno == ECONNREFUSED ) {
            close( conn_fd );
            return 0;
        }
        else {
            close( conn_fd );
            return -1;
        }
    }
    else if ( ret == 0 ) {
        printf( "port %d fond in %s \n", ntohs( serv_addr.sin_port ), inet_ntoa( serv_addr.sin_addr ) );
        close( conn_fd );
        return 1;
    }

    return -1;
}
void* scaner( void* arg ) {

    unsigned short int i;
    struct sockaddr_in serv_addr;
    port_segment       portinfo;

    /*读取端口区间信息*/
    memcpy( &portinfo, arg, sizeof( struct sockaddr_in ) );
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = portinfo.dest_ip.s_addr;

    for ( i = portinfo.min_port; i < portinfo.max_port; i++ ) {
        serv_addr.sin_port = htonl( i );
        if ( do_scan( serv_addr ) < 0 ) {
            continue;
        }
    }
}

/*
命令行参数 ：
    -m 最大端口
    -a 目标主机地址IP地址
    -n 最大线程数量
*/
int main( int argc, char const* argv[] ) {

    pthread_t*     thread;  // 指向所有的线程id
    int            max_port;
    int            thread_num;
    int            seg_len;
    struct in_addr dest_ip;  // 目标主机ip
    int            i;

    // 检查参数个数
    if ( argc != 7 ) {
        printf( "Usage:[-m][max_port][-a][serv_address][-n][thread_number]\n" );
        exit( 1 );
    }
    for ( i = 0; i < argc; i++ ) {
        if ( strcmp( "-m", argv[ i ] ) == 0 ) {
            max_port = atoi( argv[ i + 1 ] );
            if ( max_port < 0 || max_port > 65535 ) {
                printf( "Usage:invalid max dest port\n" );
                exit( 1 );
            }
            continue;
        }

        if ( strcmp( "-a", argv[ i ] ) == 0 ) {
            if ( inet_aton( argv[ i + 1 ], &dest_ip ) == 0 ) {
                printf( "Usage:invalid dest ip address\n" );
                exit( 1 );
            }
            continue;
        }

        if ( strcmp( "-n", argv[ i ] ) == 0 ) {
            thread_num = atoi( argv[ i + 1 ] );
            if ( thread_num < 0 ) {
                printf( "Usage:invalid thread num\n" );
            }
            continue;
        }
    }

    // 如果输入的最大的端口号小于线程数，则将线程数设为最大端口号
    if ( max_port < thread_num ) {
        thread_num = max_port;
    }

    seg_len = max_port / thread_num;
    printf( "thread_num = %d, max_port = %d, seg_len = %d\n", thread_num, max_port, seg_len );
    if ( ( max_port % thread_num ) != 0 ) {
        thread += 1;
    }

    // 分配内存空间
    thread = ( pthread_t* )malloc( thread_num * sizeof( pthread_t ) );
    for ( i = 0; i < thread_num; i++ ) {
        port_segment portinfo;
        portinfo.dest_ip  = dest_ip;
        portinfo.min_port = i * seg_len + 1;

        if ( i == thread_num - 1 ) {
            portinfo.max_port = max_port;
        }
        else {
            portinfo.max_port = portinfo.min_port + seg_len;
        }

        // 创建线程
        if ( pthread_create( &thread[ i ], NULL, scaner, ( void* )&portinfo ) != 0 ) {
            my_err_handler( "pthread_create", __LINE__ );
            // 主线程等待子线程结束
            pthread_join( thread[ i ], NULL );
        }
    }

    return 0;
}
