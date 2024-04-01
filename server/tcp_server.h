#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__
/* 头文件 */
#include "../common/tcp_common.h"

/* 宏定义 */
#define SERVER_EXIT_STR "server exit"
/* 类型定义 */
typedef enum 
{
    MAIN_ARGV_INDEX_IP = 1, // ip地址
    MAIN_ARGV_INDEX_MAX
}main_argv_index_t; // 传入的参数索引

typedef struct server_info
{
    bool running_flag;  // 运行标志,  == true 运行, == false 退出, 在 singnal() 修改的事件中修改

    // socket 功能相关
    int opt;
    int server_fd, new_socket;
    struct sockaddr_in address;
    char ip_address[INET_ADDRSTRLEN];
    struct sockaddr_in local_address;


}server_info_t;

#endif /* __TCP_SERVER_H__ */