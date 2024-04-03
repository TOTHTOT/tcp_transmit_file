#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__
/* 头文件 */
#include "../common/tcp_common.h"
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <pthread.h>

/* 宏定义 */
#define SERVER_EXIT_STR "server exit"
#define FILE_LISTEN_EVENT_SIZE (sizeof(struct inotify_event))
#define FILE_LISTEN_BUF_LEN (SERVER_LISTEN_MAX_FILE_NUM * (FILE_LISTEN_EVENT_SIZE + 16)) // 监听 1024 个文件

/* 类型定义 */
typedef enum
{
    MAIN_ARGV_INDEX_IP = 1,          // ip地址
    MAIN_ARGV_INDEX_POER,            // 端口号
    MAIN_ARGV_INDEX_SUB_DIR_NUM,     // 监听子文件夹数量, 校验传入参数数量时需要和 MAIN_ARGV_INDEX_MAX 一起计算
    MAIN_ARGV_INDEX_SUB_DIR,         // 监听子文件夹地址, 数量和 MAIN_ARGV_INDEX_SUB_DIR_NUM 一样
    MAIN_ARGV_INDEX_CHECK_FILE_TIME, // 轮询检测文件是否修改时间, 单位: ms
    MAIN_ARGV_INDEX_MAX
} main_argv_index_t; // 传入的参数索引

typedef struct file_listen
{
    uint8_t listen_sub_dir_num;             // 监听子文件夹数量
    int8_t *listen_sub_dir_path[UINT8_MAX]; // 监听子文件夹地址
    uint32_t check_file_time;               // 轮询检测文件是否修改时间, 单位: ms
    int inotify_fd[UINT8_MAX];              // inotify_init() 返回的文件描述符
    int inotify_wd[UINT8_MAX];              // inotify_add_watch() 返回的 watch 描述符
    int epoll_fd;                           // epoll 描述符, 用于监控多个 inotify_fd
    int pipe_fds[2];                        // 用于通知 epoll_wait()退出阻塞
} file_listen_t;

typedef struct server_info
{
    bool running_flag; // 运行标志,  == true 运行, == false 退出, 在 singnal() 修改的事件中修改

    // socket 功能相关
    int opt;
    int server_socket_fd, clent_socket_fd;
    uint32_t port;
    struct sockaddr_in address;
    char ip_address[INET_ADDRSTRLEN];

    // 监听文件夹相关功能
    file_listen_t file_listen_st;
    pthread_t file_listen_tid;
    pthread_t file_send_tid;
} server_info_t;

#endif /* __TCP_SERVER_H__ */
