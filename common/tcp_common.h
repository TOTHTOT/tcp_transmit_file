/*
 * @Description: tcp 传输文件 公共代码
 * @Author: TOTHTOT
 * @Date: 2024-04-01 16:15:37
 * @LastEditTime: 2024-04-01 21:03:43
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
#define TCP_RECV_MAX_BUFFER_SIZE (1024 * 10) // 接收缓存最大大小 1mb
#define SERVER_UPLOAD_RECENT_FILE_NUM 10     // 服务器上传最近修改过的文件数量
#define FILE_NAME_MAX_LEN 256                 // 文件名最大长度

// 日志相关函数
#if (SERVER_DEBUG_FLAG == 1)
#define INFO_PRINT(fmt, ...)                                                               \
    do                                                                                     \
    {                                                                                      \
        printf("Info %s,%s,%d: " fmt "", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)
#define ERROR_PRINT(fmt, ...)                                                               \
    do                                                                                     \
    {                                                                                      \
        printf("Error %s,%s,%d: " fmt "", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)
#else
#define INFO_PRINT(fmt, ...) \
    do                       \
    {                        \
    } while (0)
#define ERROR_PRINT(fmt, ...) \
    do                       \
    {                        \
    } while (0)
#endif /* SERVER_DEBUG_FLAG */

/* 类型定义 */
typedef struct transmit_data
{
    uint8_t file_name[FILE_NAME_MAX_LEN]; // 传输的文件名
    uint32_t file_len;                    // 传输的文件长度
    uint8_t *file_data_p;                 // 传输的文件大小
} transmit_data_t;

#endif /* __TCP_COMMON_H__ */