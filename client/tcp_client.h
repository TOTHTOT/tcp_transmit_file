#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

/* 头文件 */
#include "../common/tcp_common.h"

/* 类型定义 */
typedef enum 
{
    MAIN_ARGV_INDEX_IP = 1, // ip地址
    MAIN_ARGV_INDEX_MAX
}main_argv_index_t; // 传入的参数索引

typedef struct tcp_client_info
{
    /* sock 相关 */
    struct sockaddr_in serv_addr;
    int32_t sock;

    bool running_flag;  // 运行标志,  == true 运行, == false 退出, 在 singnal() 修改的事件中修改
}tcp_client_info_t;

#endif /* __TCP_CLIENT_H__ */