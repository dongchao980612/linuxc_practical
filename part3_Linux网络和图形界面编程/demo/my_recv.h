#ifndef __MY_RECV__H
#define __MY_RECV__H

#define BUFSIZE 1024

void my_error( const char* err_string, int line );
int my_recv( int conn_fd, char* data_buf, int len );

#endif