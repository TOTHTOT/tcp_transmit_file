/*
 * @Description: tcp 传输文件 客户端
 * @Author: TOTHTOT
 * @Date: 2024-04-02 09:03:38
 * @LastEditTime: 2024-04-04 14:07:39
 * @LastEditors: TOTHTOT
 * @FilePath: \tcp_transmit_file\client\tcp_client.h
 */
#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

/* 头文件 */
#include "../common/tcp_common.h"

/* 宏定义 */
#define CLIENT_RECV_BUF_SIZE (sizeof(transmit_data_t) * 2) // 接收缓冲区大小
#define MAX_EVENTS 10
#define TIMEOUT_MS 5000 // 超时时间 5000 毫秒

/* 类型定义 */
typedef enum
{
    MAIN_ARGV_INDEX_IP = 1,      // ip地址
    MAIN_ARGV_INDEX_POER,        // 端口号
    MAIN_ARGV_INDEX_SUB_DIR_NUM, // 监听子文件夹数量, 校验传入参数数量时需要和 MAIN_ARGV_INDEX_MAX 一起计算
    MAIN_ARGV_INDEX_SUB_DIR,     // 监听子文件夹地址, 数量和 MAIN_ARGV_INDEX_SUB_DIR_NUM 一样
    MAIN_ARGV_INDEX_MAX
} main_argv_index_t; // 传入的参数索引

typedef struct file_save_info
{
    uint8_t file_save_path_num;
    uint8_t *file_save_path[FILE_NAME_MAX_LEN];  // 文件储存地址, 具体数量由 file_save_path_num 定义
}file_save_info_t;

typedef struct tcp_client_info
{
    /* sock 相关 */
    struct sockaddr_in serv_addr;
    int32_t client_socket_fd;
    uint32_t port;
    struct sockaddr_in address;
    char ip_address[INET_ADDRSTRLEN];
    
    /* 文件传输相关 */
    char client_recv_buf[CLIENT_RECV_BUF_SIZE];
    file_save_info_t file_save_info_st;
    int32_t epoll_fd;

    bool running_flag; // 运行标志,  == true 运行, == false 退出, 在 singnal() 修改的事件中修改
} tcp_client_info_t;

#endif /* __TCP_CLIENT_H__ */