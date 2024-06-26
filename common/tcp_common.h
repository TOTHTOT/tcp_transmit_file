/*
 * @Description: tcp 传输文件 公共代码
 * @Author: TOTHTOT
 * @Date: 2024-04-01 16:15:37
 * @LastEditTime: 2024-04-04 14:51:07
 * @LastEditors: TOTHTOT
 * @FilePath: \tcp_transmit_file\common\tcp_common.h
 */
#ifndef __TCP_COMMON_H__
#define __TCP_COMMON_H__

/* 头文件 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h>

/* 宏定义 */
#define SERVER_DEBUG_FLAG 1                  // 打印是否开启
#define CLIENT_DEBUG_FLAG 1                  // 打印是否开启
#define TCP_USE_PORT 8080                    // 使用的端口
#define TRANSMIT_MAX_BUFFER_SIZE (1024 * 10) // 接收缓存最大大小 10kb
#define SERVER_UPLOAD_RECENT_FILE_NUM 10     // 服务器上传最近修改过的文件数量
#define FILE_NAME_MAX_LEN 256                // 包含地址的文件名最大长度
#define SERVER_LISTEN_MAX_FILE_NUM 1024      // 服务器监听文件数量最大值
#define SERVER_EXIT_STR "server exit"
#define CLIENT_EXIT_STR "client exit"
// 日志相关函数
#if (SERVER_DEBUG_FLAG == 1)
#define INFO_PRINT(fmt, ...)                                                               \
    do                                                                                     \
    {                                                                                      \
        printf("Info %s,%s,%d: " fmt "", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)
#define ERROR_PRINT(fmt, ...)                                                               \
    do                                                                                      \
    {                                                                                       \
        printf("Error %s,%s,%d: " fmt "", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)
#else
#define INFO_PRINT(fmt, ...) \
    do                       \
    {                        \
    } while (0)
#define ERROR_PRINT(fmt, ...) \
    do                        \
    {                         \
    } while (0)
#endif /* SERVER_DEBUG_FLAG */

/* 类型定义 */
#pragma pack(1)
typedef struct transmit_data
{
    uint8_t file_name[FILE_NAME_MAX_LEN];        // 传输的文件名
    uint8_t server_file_index;                   // 传输的文件保存索引, server 和 client 的顺序一样
    uint32_t pack_num;                           // 传输的包序号
    uint32_t file_len;                           // 传输的文件长度
    uint8_t file_data[TRANSMIT_MAX_BUFFER_SIZE]; // 传输的文件内容
} transmit_data_t;
#pragma pack()

/* 外部函数 */
extern bool is_directory_valid(const char *dir_path);
#endif /* __TCP_COMMON_H__ */