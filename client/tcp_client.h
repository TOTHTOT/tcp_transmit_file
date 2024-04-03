/*
 * @Description: tcp 传输文件 客户端
 * @Author: TOTHTOT
 * @Date: 2024-04-02 09:03:38
 * @LastEditTime: 2024-04-03 16:45:04
 * @LastEditors: TOTHTOT
 * @FilePath: \tcp_transmit_file\client\tcp_client.h
 */
#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

/* 头文件 */
#include "../common/tcp_common.h"

/* 宏定义 */
#define CLIENT_RECV_BUF_SIZE (sizeof(transmit_data_t) * 2) // 接收缓冲区大小
/* 类型定义 */
typedef enum
{
    MAIN_ARGV_INDEX_IP = 1, // ip地址
    MAIN_ARGV_INDEX_MAX
} main_argv_index_t; // 传入的参数索引

typedef struct tcp_client_info
{
    /* sock 相关 */
    struct sockaddr_in serv_addr;
    int32_t client_socket_fd;
    char client_recv_buf[CLIENT_RECV_BUF_SIZE];

    bool running_flag; // 运行标志,  == true 运行, == false 退出, 在 singnal() 修改的事件中修改
} tcp_client_info_t;

#endif /* __TCP_CLIENT_H__ */