/*
 * @Description: tcp 传输文件 公共代码
 * @Author: TOTHTOT
 * @Date: 2024-04-01 16:15:37
 * @LastEditTime: 2024-04-01 16:55:28
 * @LastEditors: TOTHTOT
 * @FilePath: \tcp_transmit_file\common\common.h
 */
#ifndef __COMMON_H__

/* 头文件 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

/* 宏定义 */
#define SERVER_DEBUG_FLAG 1                  // 打印是否开启
#define CLIENT_DEBUG_FLAG 1                  // 打印是否开启
#define TCP_USE_PORT 8080                    // 使用的端口
#define TCP_RECV_MAX_BUFFER_SIZE (1024 * 10) // 接收缓存最大大小 1mb
#define SERVER_UPLOAD_RECENT_FILE_NUM 10     // 服务器上传最近修改过的文件数量

// 日志相关函数
#if (SERVER_DEBUG_FLAG == 1)
#define INFO_PRINT(fmt, ...)                                                               \
    do                                                                                     \
    {                                                                                      \
        printf("Server Info %s,%s,%d: " fmt "", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)
#define ERROR_PRINT(fmt, ...)                                                               \
    do                                                                                     \
    {                                                                                      \
        printf("Server Error %s,%s,%d: " fmt "", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
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
#endif /* __COMMON_H__ */