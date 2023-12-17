#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <errno.h>
#include <error.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "my_recv.h"

/// @brief 错误处理函数
/// @param err_string   错误信息字符串
/// @param line         错误信息所在行
void my_error( const char* err_string, int line ) {
    fprintf( stderr, "lind:%d ", line );
    perror( err_string );
    exit( 1 );
}
/// @brief
/// @param conn_fd
/// @param data_buf
/// @param len
int my_recv( int conn_fd, char* data_buf, int len ) {

    static char  recv_buf[ BUFSIZE ];  // Custom buffer
    static char* pread      = NULL;    // Point to the next data read position
    static int   remain_len = 0;       // Number of bytes remaining in the custom buffer

    // If there is no data in the custom buffer
    // then read data from the socket
    if ( remain_len <= 0 ) {
        //  Store data into a custom buffer and update the remaining bytes
        if ( ( remain_len = recv( conn_fd, recv_buf, sizeof( recv_buf ), 0 ) ) < 0 ) {
            my_error( "recv", __LINE__ );
        }
        else if ( remain_len == 0 ) {
            //  End symbol
            return 0;
        }
        //  The pointer needs to be updated in the normal storage area
        pread = recv_buf;
    }

    //  Read data from custom buffer once
    int i;
    for ( i = 0; *pread != '\n'; ++i ) {
        if ( len < i ) {
            //  Can't read that much at once
            return -1;
        }
        data_buf[ i ] = *pread++;
        remain_len--;
    }
    // Skip end glyph
    remain_len--;
    pread++;

    //  Returns the number of bytes successfully read
    return i;
}
