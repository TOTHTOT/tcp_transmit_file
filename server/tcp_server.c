/*
 * @Description: tcp 文件传输 服务器
 * @Author: TOTHTOT
 * @Date: 2024-04-01 16:12:09
 * @LastEditTime: 2024-04-02 15:48:15
 * @LastEditors: TOTHTOT
 * @FilePath: \tcp_transmit_file\server\tcp_server.c
 */
#include "tcp_server.h"

/* 全局变量 */
server_info_t *g_server_info_st_p = NULL;
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
    INFO_PRINT("set config:\nip_address[%s], port[%s], sub_dir_num[%s], sub_dir[%s], sub_dir[%s], chek_file_time[%s]\n",
               argv[MAIN_ARGV_INDEX_IP], argv[MAIN_ARGV_INDEX_POER],
               argv[MAIN_ARGV_INDEX_SUB_DIR_NUM], argv[MAIN_ARGV_INDEX_SUB_DIR], 
               argv[MAIN_ARGV_INDEX_SUB_DIR + 1], argv[MAIN_ARGV_INDEX_SUB_DIR + 2]);

    server_info_st_p->port = atoi(argv[MAIN_ARGV_INDEX_POER]);
    server_info_st_p->file_listen_st.listen_sub_dir_num = atoi(argv[MAIN_ARGV_INDEX_SUB_DIR_NUM]);
    server_info_st_p->file_listen_st.listen_sub_dir_path[0] = (int8_t *)argv[MAIN_ARGV_INDEX_SUB_DIR];
    server_info_st_p->file_listen_st.listen_sub_dir_path[1] = (int8_t *)argv[MAIN_ARGV_INDEX_SUB_DIR + 1];
    server_info_st_p->file_listen_st.check_file_time = (int8_t *)argv[MAIN_ARGV_INDEX_SUB_DIR + 2];
    memcpy(server_info_st_p->ip_address, argv[MAIN_ARGV_INDEX_IP], INET_ADDRSTRLEN);
    INFO_PRINT("sub_dir_num = %d, path[1] = %s, path[2] = %s\n", server_info_st_p->file_listen_st.listen_sub_dir_num, 
    server_info_st_p->file_listen_st.listen_sub_dir_path[0], server_info_st_p->file_listen_st.listen_sub_dir_path[1]);
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
    if ((server_info_st_p->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 设置套接字选项
    // if (setsockopt(server_info_st_p->server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &server_info_st_p->opt, sizeof(server_info_st_p->opt)))
    if (setsockopt(server_info_st_p->server_fd, SOL_SOCKET, SO_REUSEADDR, &server_info_st_p->opt, sizeof(server_info_st_p->opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    server_info_st_p->address.sin_family = AF_INET;
    server_info_st_p->address.sin_addr.s_addr = inet_addr(server_info_st_p->ip_address);
    server_info_st_p->address.sin_port = htons(server_info_st_p->port);

    // 绑定套接字
    if (bind(server_info_st_p->server_fd, (struct sockaddr *)&server_info_st_p->address, sizeof(server_info_st_p->address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 监听
    if (listen(server_info_st_p->server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

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
    send(server_info_st_p->new_socket, SERVER_EXIT_STR, strlen(SERVER_EXIT_STR), 0);
    INFO_PRINT("server exit\n");
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
    char buffer[TCP_RECV_MAX_BUFFER_SIZE] = {0};

    g_server_info_st_p = &server_info_st;

    // 检验参数是否合法
    if (check_arg(argc, argv, &server_info_st) != 0)
    {
        ERROR_PRINT("check_arg() fail, exit program!\n");
        return 1;
    }

    // 注册信号处理函数
    signal(SIGINT, sig_handler);

    // 初始化服务器 sock 相关功能
    tcp_server_sock_init(g_server_info_st_p);

    // 阻塞接受连接
    if ((server_info_st.new_socket = accept(server_info_st.server_fd, (struct sockaddr *)&server_info_st.address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    INFO_PRINT("client connected, client_fd = %d\n", server_info_st.new_socket);
    // 主循环, 处理发送功能
    server_info_st.running_flag = true;
    while (server_info_st.running_flag == true)
    {
        scanf("%s", buffer);
        // 发送消息给客户端
        send(server_info_st.new_socket, buffer, strlen(buffer), 0);
        INFO_PRINT("is running\n");
        usleep(1000 * 1000); // 延时 1s
    }

    server_exit(g_server_info_st_p);
    return 0;
}
