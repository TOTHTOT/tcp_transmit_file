/*
 * @Description: tcp 传输文件 客户端
 * @Author: TOTHTOT
 * @Date: 2024-04-01 16:12:31
 * @LastEditTime: 2024-04-05 09:23:38
 * @LastEditors: TOTHTOT
 * @FilePath: \tcp_transmit_file\client\tcp_client.c
 */
/* 头文件 */
#include "tcp_client.h"
#include <sys/epoll.h>

/* 全局变量 */
tcp_client_info_t *g_client_info_st_p = NULL;

/* 内部函数声明 */
static uint8_t client_exit(tcp_client_info_t *client_info_st_p);

/**
 * @name: client_set_conifg
 * @msg: 客户端保存参数
 * @param {char} *argv
 * @param {tcp_client_info_t} *client_info_st_p
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-04-04 14:13:23
 */
static uint8_t client_set_conifg(char *argv[], tcp_client_info_t *client_info_st_p)
{
    client_info_st_p->port = atoi(argv[MAIN_ARGV_INDEX_POER]);
    client_info_st_p->file_save_info_st.file_save_path_num = atoi(argv[MAIN_ARGV_INDEX_SUB_DIR_NUM]);

    // 根据 file_save_path_num 填写 file_save_path
    for (uint8_t i = 0; i < client_info_st_p->file_save_info_st.file_save_path_num; i++)
    {
        client_info_st_p->file_save_info_st.file_save_path[i] = (uint8_t *)argv[MAIN_ARGV_INDEX_SUB_DIR + i];
        // INFO_PRINT("file_save_path[%d] = %s\n", i, argv[MAIN_ARGV_INDEX_SUB_DIR + i]);
    }

    memcpy(client_info_st_p->ip_address, argv[MAIN_ARGV_INDEX_IP], INET_ADDRSTRLEN);

    INFO_PRINT("set config:\nport = %d, sub_dir_num = %d\npath[1] = %s\npath[2] = %s\n",
               client_info_st_p->port,
               client_info_st_p->file_save_info_st.file_save_path_num,
               client_info_st_p->file_save_info_st.file_save_path[0],
               client_info_st_p->file_save_info_st.file_save_path[1]);

    return 0;
}

/**
 * @name: check_arg
 * @msg: 检测参数是否有效
 * @param {int} argc
 * @param {char} *argv
 * @param {tcp_client_info_t} *client_info_st_p
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-04-04 10:13:55
 */
static uint8_t check_arg(int argc, char *argv[], tcp_client_info_t *client_info_st_p)
{
    main_argv_index_t index_em = (main_argv_index_t)argc;
    struct in_addr addr;
    int32_t ret = 0;

    // 参数数量不合法
    if (index_em < MAIN_ARGV_INDEX_MAX)
    {
        ERROR_PRINT("argc invalid[%d]\n", argc);
        ret = 1;
        goto ERROR_PRINT;
    }

    // 检测IP地址是否合法
    ret = inet_pton(AF_INET, argv[MAIN_ARGV_INDEX_IP], &addr);
    if (ret != 1)
    {
        ERROR_PRINT("ip_address invalid[%s]\n", argv[MAIN_ARGV_INDEX_IP]);
        ret = 2;
        goto ERROR_PRINT;
    }
    // 检测文件夹地址是否有效
    for (int32_t i = 0; i < atoi(argv[MAIN_ARGV_INDEX_SUB_DIR_NUM]); i++)
    {
        if (is_directory_valid(argv[MAIN_ARGV_INDEX_SUB_DIR + i]) == false)
        {
            ERROR_PRINT("sub_dir invalid[%s]\n", argv[MAIN_ARGV_INDEX_SUB_DIR + i]);
            return 3;
        }
    }
    client_set_conifg(argv, client_info_st_p);
    return 0;

ERROR_PRINT:
    printf("Usage: <IP_ADDRESS>\n");
    printf("Example: 192.168.1.1\n");
    // ERROR_RETURN:
    return ret;
}

/**
 * @name: client_save_file
 * @msg: 客户端保存接收到的数据到文件, 因为是分包发送所以第一次接收到数据时
 * 删除源文件创建新文件再追加内容到文件, 通过包序号判断是否第一包
 * @param {char} *file_path 文件保存地址
 * @param {char} *file_name 文件名称
 * @param {char} *file_data 文件数据
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-04-03 14:08:17
 */
uint8_t client_save_file(const char *file_path, const char *file_name, const char *file_data)
{
    char full_path[FILE_NAME_MAX_LEN];
    snprintf(full_path, sizeof(full_path), "%s/%s", file_path, file_name);

    FILE *file = fopen(full_path, "a"); // 以追加模式打开文件
    if (file == NULL)
    {
        perror("client_save_file()fopen");
        return 1; // 打开文件失败
    }

    // 写入数据到文件
    if (fprintf(file, "%s", file_data) < 0)
    {
        perror("fprintf");
        fclose(file);
        return 2; // 写入数据失败
    }

    // 关闭文件
    fclose(file);
    return 0; // 操作成功
}

/**
 * @name: client_analysis_server_pack
 * @msg: 客户端分析服务器分包数据, 服务器发送的包长固定
 * @param {char} *buffer 接收缓存
 * @param {uint32_t} buffer_size 接收缓存大小
 * @param {tcp_client_info_t} *client_info_st_p client 结构体
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-04-03 15:09:38
 */
uint8_t client_analysis_server_pack(char *buffer, uint32_t buffer_size, tcp_client_info_t *client_info_st_p)
{
    char *file_path = NULL; // 文件保存地址 根据 server_file_index 得到
    char *file_name = NULL; // 文件名称 解包得到
    uint32_t package_count = buffer_size / sizeof(transmit_data_t);

    // 每包长度都是固定的, 如果除不尽就说明有问题
    if (buffer_size % sizeof(transmit_data_t) != 0)
        return 1; // 数据包长度错误

    // 分包处理
    for (uint32_t i = 0; i < package_count; i++)
    {
        transmit_data_t *transmit_data_st_p = (transmit_data_t *)(buffer + i * sizeof(transmit_data_t));
        if (transmit_data_st_p->pack_num == 0)
            INFO_PRINT("buffer_size = %d,file_name= %s\n\n", buffer_size, transmit_data_st_p->file_name);

        // 根据 server_file_index 选择保存地址
        file_path = (char *)client_info_st_p->file_save_info_st.file_save_path[transmit_data_st_p->server_file_index];
        file_name = (char *)transmit_data_st_p->file_name;
        INFO_PRINT("\nbuffer_size = %d, pack_num = %d, file_name= %s\n",
                   buffer_size, transmit_data_st_p->pack_num, transmit_data_st_p->file_name);

        // 如果是第 0 包数据就删除源文件并创建新文件
        if (transmit_data_st_p->pack_num == 0)
        {
            char full_path[FILE_NAME_MAX_LEN * 2];
            snprintf(full_path, sizeof(full_path), "%s/%s", file_path, file_name);
            remove(full_path);
            // 创建新文件
            FILE *file = fopen(full_path, "w"); // 使用写入模式打开文件，会创建新文件或清空已存在的文件
            if (file == NULL)
            {
                ERROR_PRINT("fopen error open path = %s \n", full_path);
                perror("client_analysis_server_pack() fopen");
                return 2; // 返回 0 表示打开文件失败
            }
            // 关闭文件
            fclose(file);
        }
        // 保存数据到文件
        client_save_file(file_path, file_name, (char *)transmit_data_st_p->file_data);
    }
    return 0;
}

/**
 * @name: clinet_socket_init
 * @msg: 客户端初始化 socket
 * @param {tcp_client_info_t} *client_info_st_p
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-04-04 14:17:21
 */
uint8_t clinet_socket_init(tcp_client_info_t *client_info_st_p)
{
    uint32_t connect_retry_times = 0; // 连接重试次数
    // 创建套接字
    if ((client_info_st_p->client_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        INFO_PRINT("\n Socket creation error \n");
        return 1;
    }
// 将套接字设置为阻塞模式
    if (fcntl(client_info_st_p->client_socket_fd, F_SETFL, 0) == -1)
    {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    client_info_st_p->serv_addr.sin_family = AF_INET;
    client_info_st_p->serv_addr.sin_port = htons(client_info_st_p->port);

    // 将 IPv4 地址从文本转换为二进制
    if (inet_pton(AF_INET, client_info_st_p->ip_address, &client_info_st_p->serv_addr.sin_addr) <= 0)
    {
        INFO_PRINT("\nInvalid address/ Address not supported \n");
        return 2;
    }

    // 连接服务器
    while (client_info_st_p->running_flag == true)
    {
        // 连接服务器
        if (connect(client_info_st_p->client_socket_fd, (struct sockaddr *)&client_info_st_p->serv_addr, sizeof(client_info_st_p->serv_addr)) < 0)
        {
            INFO_PRINT("Connection Failed Server offline, retry times = %d\n", connect_retry_times);
        }
        else
        {
                        // 连接成功退出
            return 0;
        }
        connect_retry_times++;

        INFO_PRINT("connect retry times = %d\r", connect_retry_times);
        fflush(stdout);
        usleep(100 * 1000);
    }

        // 没连接上 服务器退出流程
    client_exit(client_info_st_p);
    return 3;
}

/**
 * @name: sig_handler
 * @msg: 信号处理函数
 * @param {int} signum 信号
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-04-04 14:24:56
 */
void sig_handler(int signum)
{
    if (signum == SIGINT)
    {
        /* if (g_client_info_st_p->running_flag == false)
        {
            INFO_PRINT("no client linked, exit!\n");
            exit(EXIT_SUCCESS);
        }
        else */
        {
            // server 退出
            g_client_info_st_p->running_flag = false;
        }
        INFO_PRINT("SIGINT\n");
    }
}

/**
 * @name: client_init
 * @msg: 初始化客户端程序
 * @param {tcp_client_info_t} *client_info_st_p
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-04-04 14:21:43
 */
uint8_t client_init(tcp_client_info_t *client_info_st_p)
{
    // 初始化 socket
    if (clinet_socket_init(client_info_st_p) != 0)
    {
        ERROR_PRINT("clinet_socket_init error\n");
        return 1; // 初始化失败
    }

    // 注册信号处理函数
    signal(SIGINT, sig_handler);

    // 创建 epoll 管理 socket 的文件描述符实现打段 recv
    client_info_st_p->epoll_fd = epoll_create1(0);
    if (client_info_st_p->epoll_fd == -1)
    {
        perror("epoll_create1");
        return 2;
    }

    // 添加 sockfd 到 epoll 实例中
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = client_info_st_p->client_socket_fd;
    if (epoll_ctl(client_info_st_p->epoll_fd, EPOLL_CTL_ADD, client_info_st_p->client_socket_fd, &event) == -1)
    {
        perror("epoll_ctl");
        return 3;
    }
    return 0;
}

/**
 * @name: client_exit
 * @msg: 客户端退出处理函数, 关闭打开的描述符等等操作
 * @param {tcp_client_info_t} *client_info_st_p
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-04-04 14:47:17
 */
static uint8_t client_exit(tcp_client_info_t *client_info_st_p)
{
    // 发送消息给服务器 client 关闭
    send(client_info_st_p->client_socket_fd, CLIENT_EXIT_STR, strlen(CLIENT_EXIT_STR), 0);
    // 关闭套接字
    close(client_info_st_p->client_socket_fd);

    INFO_PRINT("client exit\n");

    return 0;
}

/**
 * @name: main
 * @msg:
 * @param {int} argc
 * @param {char} *argv
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-04-03 15:10:56
 */
int main(int argc, char *argv[])
{
    tcp_client_info_t client_info_st = {0};
    int valread = 0;

    printf("\nclient version date %s %s\n\n", __DATE__, __TIME__);
    g_client_info_st_p = &client_info_st;

    client_info_st.running_flag = true;
    // 校验参数
    if (check_arg(argc, argv, &client_info_st) != 0)
    {
        ERROR_PRINT("check_arg error\n");
        exit(EXIT_FAILURE);
    }
    // 初始化客户端
    if (client_init(&client_info_st) != 0)
    {
        ERROR_PRINT("client_init error\n");
        exit(EXIT_FAILURE);
    }

    while (client_info_st.running_flag == true)
    {
        // 使用 epoll 等待事件
        struct epoll_event events[MAX_EVENTS];
        int num_events = epoll_wait(client_info_st.epoll_fd, events, MAX_EVENTS, TIMEOUT_MS);
        if (num_events == -1)
        {
            perror("epoll_wait");
            return 1;
        }
        else if (num_events == 0)
        {
            // 超时处理
            // printf("Timeout occurred. No data received.\n");
        }
        else
        {
            // 有事件发生
            for (int i = 0; i < num_events; ++i)
            {
                if (events[i].data.fd == client_info_st.client_socket_fd && events[i].events & EPOLLIN)
                {
                    // 接收消息
                    valread = recv(client_info_st.client_socket_fd, client_info_st.client_recv_buf, CLIENT_RECV_BUF_SIZE, 0);
                    if (valread == -1) // 接收错误
                    {
                        perror("recv");
                        break;
                    }
                    else if (valread == 0) // 断开连接
                    {
                        printf("Connection closed by peer\n");
                        break;
                    }
                    else // 处理接收数据
                    {
                        client_analysis_server_pack(client_info_st.client_recv_buf, valread, &client_info_st);
                        memset(client_info_st.client_recv_buf, 0, CLIENT_RECV_BUF_SIZE);
                    }
                }
            }
        }
    }

    client_exit(&client_info_st);

    return 0;
}
