#ifndef __SERVER_H__

/* 头文件 */
#include "../common/common.h"

/* 类型定义 */
typedef enum 
{
    MAIN_ARGV_INDEX_IP = 0, // ip地址
    MAIN_ARGV_INDEX_MAX
}main_argv_index_t; // 传入的参数索引

typedef struct server_info
{
    bool running_flag;  // 运行标志,  == true 运行, == false 退出, 在 singnal() 修改的事件中修改
    int server_fd, new_socket;
    struct sockaddr_in address;
}server_info_t;

#endif /* __SERVER_H__ */