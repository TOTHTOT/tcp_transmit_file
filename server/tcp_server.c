/*
 * @Description: tcp 文件传输 服务器
 * @Author: TOTHTOT
 * @Date: 2024-04-01 16:12:09
 * @LastEditTime: 2024-04-03 14:50:24
 * @LastEditors: TOTHTOT
 * @FilePath: \tcp_transmit_file\server\tcp_server.c
 */
#include "tcp_server.h"

/* 全局变量 */
server_info_t *g_server_info_st_p = NULL;

/**
 * @name: server_set_conifg
 * @msg: 将传入的 argv 保存到使用的参数中
 * @param {char} *argv
 * @param {server_info_t} *server_info_st_p
 * @return {== 0 成功}
 * @author: TOTHTOT
 * @Date: 2024-04-02 15:53:03
 */
static uint8_t server_set_conifg(char *argv[], server_info_t *server_info_st_p)
{
    server_info_st_p->port = atoi(argv[MAIN_ARGV_INDEX_POER]);
    server_info_st_p->file_listen_st.listen_sub_dir_num = atoi(argv[MAIN_ARGV_INDEX_SUB_DIR_NUM]);
    server_info_st_p->file_listen_st.listen_sub_dir_path[0] = (int8_t *)argv[MAIN_ARGV_INDEX_SUB_DIR];
    server_info_st_p->file_listen_st.listen_sub_dir_path[1] = (int8_t *)argv[MAIN_ARGV_INDEX_SUB_DIR + 1];
    server_info_st_p->file_listen_st.check_file_time = atoi(argv[MAIN_ARGV_INDEX_SUB_DIR + 2]);
    memcpy(server_info_st_p->ip_address, argv[MAIN_ARGV_INDEX_IP], INET_ADDRSTRLEN);

    INFO_PRINT("port = %d, sub_dir_num = %d, path[1] = %s, path[2] = %s time = %d\n",
               server_info_st_p->port, server_info_st_p->file_listen_st.listen_sub_dir_num,
               server_info_st_p->file_listen_st.listen_sub_dir_path[0], server_info_st_p->file_listen_st.listen_sub_dir_path[1],
               server_info_st_p->file_listen_st.check_file_time);

    return 0;
}
/**
 * @name: check_arg
 * @msg: 检测传入参数是否合法
 * @param {int} argc 参数数量
 * @param {char} *argv 参数值
 * @return { == 0, 成功; == 1 失败}
 * @author: TOTHTOT
 * @Date: 2024-04-01 16:50:58
 */
uint8_t check_arg(int argc, char *argv[], server_info_t *server_info_st_p)
{
    main_argv_index_t index_em = (main_argv_index_t)argc;
    struct in_addr addr;
    int32_t ret = 0;
    if (index_em < MAIN_ARGV_INDEX_MAX) // 参数数量大于等于最大值, 错误
    {
        ERROR_PRINT("argc invalid[%d]\n", argc);
        ret = 1;
        goto ERROR_PRINT;
    }
    // 检测IP地址是否有效
    ret = inet_pton(AF_INET, argv[MAIN_ARGV_INDEX_IP], &addr);
    if (ret != 1)
    {
        ERROR_PRINT("ip_address invalid[%s]\n", argv[MAIN_ARGV_INDEX_IP]);
        ret = 2;
        goto ERROR_PRINT;
    }
    // 校验输入参数, 设置参数
    /* INFO_PRINT("set config:\nip_address[%s], port[%s], sub_dir_num[%s], sub_dir[%s], sub_dir[%s], chek_file_time[%s]\n",
               argv[MAIN_ARGV_INDEX_IP], argv[MAIN_ARGV_INDEX_POER],
               argv[MAIN_ARGV_INDEX_SUB_DIR_NUM], argv[MAIN_ARGV_INDEX_SUB_DIR],
               argv[MAIN_ARGV_INDEX_SUB_DIR + 1], argv[MAIN_ARGV_INDEX_SUB_DIR + 2]); */

    server_set_conifg(argv, server_info_st_p);
    return 0;

ERROR_PRINT:
    printf("Usage: <IP_ADDRESS>\n");
    printf("Example: 192.168.1.1\n");
    // ERROR_RETURN:
    return ret;
}

/**
 * @name: sig_handler
 * @msg: 信号处理函数
 * @param {int} signum 信号值
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-04-01 20:44:47
 */
void sig_handler(int signum)
{
    if (signum == SIGINT)
    {
        // 接收到 SIGINT 时 running_flag 还是 false, 表示还没有 client 连接,直接退出
        if (g_server_info_st_p->running_flag == false)
        {
            INFO_PRINT("no client linked, exit!\n");
            exit(EXIT_SUCCESS);
        }
        else
        {
            // server 退出
            g_server_info_st_p->running_flag = false;
        }
        INFO_PRINT("SIGINT\n");
    }
}

/**
 * @name: tcp_server_sock_init
 * @msg: tcp 服务器 sock 功能初始化
 * @param {server_info_t} *server_info_st_p
 * @return {== 0, 成功; != 0, 失败, 程序直接退出;}
 * @author: TOTHTOT
 * @Date: 2024-04-01 20:46:43
 */
uint8_t tcp_server_sock_init(server_info_t *server_info_st_p)
{
    server_info_st_p->opt = 1;

    // 创建套接字
    if ((server_info_st_p->clent_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 设置套接字选项
    // if (setsockopt(server_info_st_p->clent_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &server_info_st_p->opt, sizeof(server_info_st_p->opt)))
    if (setsockopt(server_info_st_p->clent_socket_fd, SOL_SOCKET, SO_REUSEADDR, &server_info_st_p->opt, sizeof(server_info_st_p->opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    server_info_st_p->address.sin_family = AF_INET;
    server_info_st_p->address.sin_addr.s_addr = inet_addr(server_info_st_p->ip_address);
    server_info_st_p->address.sin_port = htons(server_info_st_p->port);

    // 绑定套接字
    if (bind(server_info_st_p->clent_socket_fd, (struct sockaddr *)&server_info_st_p->address, sizeof(server_info_st_p->address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 监听
    if (listen(server_info_st_p->clent_socket_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    return 0;
}

/**
 * @name: server_file_listen_init
 * @msg: 服务器监听文件功能初始化, 使用 epoll 管理多个 inotify_fd
 * @param {server_info_t} *server_info_st_p
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-04-02 17:14:16
 */
uint8_t server_file_listen_init(server_info_t *server_info_st_p)
{
    int32_t i = 0;

    // 初始化 epoll
    server_info_st_p->file_listen_st.epoll_fd = epoll_create1(0);
    if (server_info_st_p->file_listen_st.epoll_fd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < server_info_st_p->file_listen_st.listen_sub_dir_num; i++)
    {
        server_info_st_p->file_listen_st.inotify_fd[i] = inotify_init();
        if (server_info_st_p->file_listen_st.inotify_fd[i] < 0)
        {
            ERROR_PRINT("inotify_init()[%d] error\n", i);
            return 1;
        }

        // 根据传入的监听目录数量设置对应的 watch 描述符, 这里监听文件是否修改
        server_info_st_p->file_listen_st.inotify_wd[i] = inotify_add_watch(server_info_st_p->file_listen_st.inotify_fd[i], (char *)server_info_st_p->file_listen_st.listen_sub_dir_path[i], IN_MODIFY);
        if (server_info_st_p->file_listen_st.inotify_wd[i] < 0)
        {

            ERROR_PRINT("inotify_add_watch()[%d] error\n", i);
            return 2;
        }

        // 添加 epoll 监听事件
        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = server_info_st_p->file_listen_st.inotify_fd[i];

        if (epoll_ctl(server_info_st_p->file_listen_st.epoll_fd, EPOLL_CTL_ADD, server_info_st_p->file_listen_st.inotify_fd[i], &event) == -1)
        {
            ERROR_PRINT("epoll_ctl() fail\n");
            return 3;
        }
    }

    // 创建管道
    if (pipe(server_info_st_p->file_listen_st.pipe_fds) == -1)
    {
        perror("pipe");
        return 4;
    }

    // 添加 epoll 监听事件, 监听 pipe
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_info_st_p->file_listen_st.pipe_fds[0];
    if (epoll_ctl(server_info_st_p->file_listen_st.epoll_fd, EPOLL_CTL_ADD, server_info_st_p->file_listen_st.pipe_fds[0], &event) == -1)
    {
        ERROR_PRINT("epoll_ctl() fail\n");
        return 3;
    }

    return 0;
}

/**
 * @name: server_init
 * @msg: 服务器所有的初始化都放到里面
 * @param {int} argc
 * @param {char} *argv
 * @param {server_info_t} *server_info_st_p
 * @return { == 0 成功; != 0 失败}
 * @author: TOTHTOT
 * @Date: 2024-04-02 15:55:52
 */
uint8_t server_init(int argc, char *argv[], server_info_t *server_info_st_p)
{
    // 检验参数是否合法
    if (check_arg(argc, argv, server_info_st_p) != 0)
    {
        ERROR_PRINT("check_arg() fail, exit program!\n");
        return 1;
    }

    // 注册信号处理函数
    signal(SIGINT, sig_handler);

    // 初始化服务器 sock 相关功能, 有异常直接退出
    if (tcp_server_sock_init(server_info_st_p) != 0)
        return 2;
    return 0;
}
/**
 * @name: server_exit
 * @msg: 服务器关闭
 * @param {server_info_t} *server_info_st_p
 * @return {void}
 * @author: TOTHTOT
 * @Date: 2024-04-01 20:48:48
 */
void server_exit(server_info_t *server_info_st_p)
{
    uint32_t i = 0;
    char buf = 1;

    // 关闭 inotify_fd
    for (i = 0; i < server_info_st_p->file_listen_st.listen_sub_dir_num; i++)
    {
        close(server_info_st_p->file_listen_st.inotify_fd[i]);
    }
    // 关闭 epoll, 线程会立马退出
    close(server_info_st_p->file_listen_st.epoll_fd);

    // 通过管道发送数据给 epoll_wait 实现打断阻塞
    write(server_info_st_p->file_listen_st.pipe_fds[1], &buf, sizeof(buf));
    // 回收线程
    pthread_join(server_info_st_p->file_listen_tid, NULL);

    send(server_info_st_p->clent_socket_fd, SERVER_EXIT_STR, strlen(SERVER_EXIT_STR), 0);
    INFO_PRINT("server exit\n");
}

/**
 * @name: server_send_one_file
 * @msg: 服务器 向 client 发送单个文件
 * @param {server_info_t} *server_info_st_p 服务器结构体
 * @param {transmit_data_t} *transmit_data_st_p 传输文件结构体指针
 * @return { == 0 成功; != 0 失败;}
 * @author: TOTHTOT
 * @Date: 2024-04-03 11:31:52
 */
uint8_t server_send_one_file(server_info_t *server_info_st_p, transmit_data_t *transmit_data_st_p)
{
    FILE *file = fopen((char *)transmit_data_st_p->server_file_path, "rb");
    if (file == NULL)
    {
        perror("fopen");
        return 1; // 返回 1 表示打开失败
    }

    INFO_PRINT("start send file: %s\n", transmit_data_st_p->server_file_path);
    // 从文件中读取数据并发送
    size_t bytes_read;
    while ((bytes_read = fread(transmit_data_st_p->file_data, 1, TRANSMIT_MAX_BUFFER_SIZE, file)) > 0)
    {
        // 将整个结构体发出去
        if (send(server_info_st_p->clent_socket_fd, transmit_data_st_p, sizeof(transmit_data_t), 0) == -1)
        {
            perror("send");
            ERROR_PRINT("send file: %s fail\n", transmit_data_st_p->server_file_path);
            fclose(file);
            return 2;
        }
        memset(transmit_data_st_p->file_data, 0, TRANSMIT_MAX_BUFFER_SIZE);
        INFO_PRINT("send file: %s, %d bytes, pack_num = %d\n", transmit_data_st_p->server_file_path, bytes_read, transmit_data_st_p->pack_num);
        transmit_data_st_p->pack_num++;
    }

    free(transmit_data_st_p);
    // 关闭文件
    fclose(file);
    INFO_PRINT("send finish\n");
    return 0;
}
/**
 * @name: pth_file_listen
 * @msg: 监听文件是否状态线程
 * @param {void} *arg
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-04-02 17:05:24
 */
void *pth_file_listen(void *arg)
{
    server_info_t *server_info_st_p = (server_info_t *)arg;
    // 初始化监听文件
    if (server_file_listen_init(server_info_st_p) != 0)
    {
        ERROR_PRINT("server_file_listen_init() fail, exit program!\n");
        return NULL;
    }
    // 根据监听文件夹数申请内存
    struct epoll_event *events = malloc(sizeof(struct epoll_event) * server_info_st_p->file_listen_st.listen_sub_dir_num);
    if (events == NULL)
    {
        ERROR_PRINT("malloc() fail, exit pth_file_listen!\n");
        return NULL;
    }
    INFO_PRINT("epoll_fd = %d, listen_num = %d\n", server_info_st_p->file_listen_st.epoll_fd,
               server_info_st_p->file_listen_st.listen_sub_dir_num);
    while (1)
    {
        int num_events = epoll_wait(server_info_st_p->file_listen_st.epoll_fd, events, server_info_st_p->file_listen_st.listen_sub_dir_num, -1);
        if (num_events == -1)
        {
            perror("epoll_wait() fail");
            break;
        }

        // 优先处理管道发来的数据, 收到就退出
        if (events[0].data.fd == server_info_st_p->file_listen_st.pipe_fds[0])
        {
            // INFO_PRINT("Received signal, exiting...\n");
            break;
        }

        // 处理监听文件事件
        for (int i = 0; i < num_events; i++)
        {
            for (int j = 0; j < server_info_st_p->file_listen_st.listen_sub_dir_num; j++)
            {
                if (events[i].data.fd == server_info_st_p->file_listen_st.inotify_fd[j])
                {
                    INFO_PRINT("Inotify event occurred for inotify_fd %d, path = %s\n", server_info_st_p->file_listen_st.inotify_fd[j], server_info_st_p->file_listen_st.listen_sub_dir_path[j]);

                    // 处理文件系统事件

                    // 读取inotify_fd以清除事件
                    char buf[FILE_LISTEN_BUF_LEN] = {0};
                    ssize_t len, ii = 0;
                    len = read(server_info_st_p->file_listen_st.inotify_fd[j], buf, FILE_LISTEN_BUF_LEN);
                    if (len < 0)
                    {
                        ERROR_PRINT("read() fail\n");
                        break;
                    }
                    struct inotify_event *event = (struct inotify_event *)&buf[ii];
                    if (event->mask & IN_MODIFY)
                    {
                        transmit_data_t *temp_transmit_data_p = malloc(sizeof(transmit_data_t));    // 传输文件功能结构体, 发送完成后释放掉
                        if (temp_transmit_data_p == NULL)
                        {
                            ERROR_PRINT("malloc() to temp_transmit_data_p fail!!\n");
                            break;
                        }

                        INFO_PRINT("File modified: %s\n", event->name);

                        // 合并地址和文件名
                        sprintf((char *)temp_transmit_data_p->server_file_path, "%s/%s", server_info_st_p->file_listen_st.listen_sub_dir_path[j], event->name);
                        sprintf((char *)temp_transmit_data_p->file_name, "%s", event->name);
                        // 添加到传输列表, 同时传输多个文件时可能会由于网络原因造成阻塞所以使用列表
                        server_send_one_file(server_info_st_p, temp_transmit_data_p);
                    }

                    /* while (ii < len)
                    {
                        ii += FILE_LISTEN_EVENT_SIZE + event->len;
                    } */
                }
            }
        }
    }

    free(events);
    // INFO_PRINT("thread end\n");
    return NULL;
}

/**
 * @name: pth_file_send
 * @msg: 发送文件线程
 * @param {void} *arg
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-04-03 10:58:30
 */
void *pth_file_send(void *arg)
{
    while (1)
    {
        usleep(100);
    }
}
/**
 * @name: main
 * @msg:
 * @param {int} argc
 * @param {char} *argv
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-04-01 21:00:02
 */
int main(int argc, char *argv[])
{
    server_info_t server_info_st = {0};
    int addrlen = sizeof(server_info_st.address);

    g_server_info_st_p = &server_info_st;

    // 初始化服务器
    if (server_init(argc, argv, &server_info_st) != 0)
    {
        ERROR_PRINT("server_init() fail, exit program!\n");
        return 1;
    }

    // 阻塞接受连接
    if ((server_info_st.clent_socket_fd = accept(server_info_st.clent_socket_fd, (struct sockaddr *)&server_info_st.address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    // 连接成功创建线程监听文件
    pthread_create(&server_info_st.file_listen_tid, NULL, pth_file_listen, (void *)&server_info_st);
    // 创建发送文件线程
    pthread_create(&server_info_st.file_send_tid, NULL, pth_file_send, (void *)&server_info_st);
    
    INFO_PRINT("client connected, client_fd = %d\n", server_info_st.clent_socket_fd);
    // 主循环, 处理发送功能
    server_info_st.running_flag = true;
    while (server_info_st.running_flag == true)
    {
        /* ssize_t len, i = 0;
        len = read(server_info_st.file_listen_st.inotify_fd, listen_file_buffer, FILE_LISTEN_BUF_LEN);
        if (len < 0)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }

        while (i < len)
        {
            struct inotify_event *event = (struct inotify_event *)&listen_file_buffer[i];
            if (event->mask & IN_MODIFY )
            {
                printf("File modified: %s\n", event->name);
            }
            i += FILE_LISTEN_EVENT_SIZE + event->len;
        } */

#if 0
        scanf("%s", buffer);
        // 发送消息给客户端
        send(server_info_st.clent_socket_fd, buffer, strlen(buffer), 0);
        INFO_PRINT("is running\n");
#endif
        usleep(1000 * 1000); // 延时 1s
    }

    server_exit(g_server_info_st_p);
    return 0;
}
