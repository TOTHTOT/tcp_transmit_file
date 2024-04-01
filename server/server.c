/*
 * @Description: tcp 文件传输 服务器
 * @Author: TOTHTOT
 * @Date: 2024-04-01 16:12:09
 * @LastEditTime: 2024-04-01 17:13:10
 * @LastEditors: TOTHTOT
 * @FilePath: \tcp_transmit_file\server\server.c
 */
#include "server.h"

/**
 * @name: check_arg
 * @msg: 检测传入参数是否合法
 * @param {int} argc 参数数量
 * @param {char} *argv 参数值
 * @return { == 0, 成功; == 1 失败}
 * @author: TOTHTOT
 * @Date: 2024-04-01 16:50:58
 */
uint8_t check_arg(int argc, char *argv[])
{
    main_argv_index_t index_em = (main_argv_index_t)argc;

    if (index_em >= MAIN_ARGV_MAX_NUM) // 参数数量大于等于最大值, 错误
    {
        ERROR_PRINT("argc invalid[%d]\n", arc);
        return 1;
    }
    
    return 0;
}

int main(int argc, char *argv[])
{
    server_info_t server_info_st = {
        .running_flag = true,
    };
    int opt = 1;
    int addrlen = sizeof(server_info_st.address);
    char buffer[TCP_RECV_MAX_BUFFER_SIZE] = {0};
    const char *message = "Hello from server";

    // 检验参数是否合法
    if (check_arg(argc, argv) != 0)
    {
        ERROR_PRINT("check_arg() fail, exit program!\n");
        return 1;
    }
    
    // 创建套接字
    if ((server_info_st.server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 设置套接字选项
    if (setsockopt(server_info_st.server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    server_info_st.address.sin_family = AF_INET;
    server_info_st.address.sin_addr.s_addr = INADDR_ANY;
    server_info_st.address.sin_port = htons(TCP_USE_PORT);

    // 绑定套接字
    if (bind(server_info_st.server_fd, (struct sockaddr *)&server_info_st.address, sizeof(server_info_st.address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 监听
    if (listen(server_info_st.server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    while (server_info_st.running_flag == true)
    {
        // 接受连接
        if ((server_info_st.new_socket = accept(server_info_st.server_fd, (struct sockaddr *)&server_info_st.address, (socklen_t *)&server_info_st.addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
    }
    

    // 发送消息给客户端
    send(server_info_st.new_socket, message, strlen(message), 0);
    printf("Hello message sent\n");
    return 0;
}
