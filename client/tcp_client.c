/*
 * @Description: tcp 传输文件 客户端
 * @Author: TOTHTOT
 * @Date: 2024-04-01 16:12:31
 * @LastEditTime: 2024-04-01 20:40:49
 * @LastEditors: TOTHTOT
 * @FilePath: \tcp_transmit_file\client\tcp_client.c
 */
#include "tcp_client.h"

#define BUFFER_SIZE 100

int main(int argc, char *argv[])
{
    tcp_client_info_t client_info_st = {
        .running_flag = true,
    };
    int valread;
    char buffer[BUFFER_SIZE] = {0};
    const char *hello = "Hello from client";
    uint8_t connect_retry_times = 0;  // 连接重试次数
    // 创建套接字
    if ((client_info_st.sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        INFO_PRINT("\n Socket creation error \n");
        return -1;
    }
     // 将套接字设置为阻塞模式
    if (fcntl(client_info_st.sock, F_SETFL, 0) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }


    client_info_st.serv_addr.sin_family = AF_INET;
    client_info_st.serv_addr.sin_port = htons(TCP_USE_PORT);

    // 将 IPv4 地址从文本转换为二进制
    if (inet_pton(AF_INET, "127.0.0.1", &client_info_st.serv_addr.sin_addr) <= 0)
    {
        INFO_PRINT("\nInvalid address/ Address not supported \n");
        return -1;
    }
    while (connect_retry_times <= 100)
    {
        // 连接到服务器
        if (connect(client_info_st.sock, (struct sockaddr *)&client_info_st.serv_addr, sizeof(client_info_st.serv_addr)) < 0)
        {
            INFO_PRINT("Connection Failed Server offline, retry times = %d\n", connect_retry_times);
        }
        else
        {
            break;
        }
        connect_retry_times++;
        usleep(100*1000);
    }
    if (connect_retry_times >= 100)
    {
        INFO_PRINT("\nConnection Failed Server offline, clien exit!\n");
        return 1;
    }
    
    while (client_info_st.running_flag)
    {
        // 接收消息
        valread = read(client_info_st.sock, buffer, BUFFER_SIZE);
        INFO_PRINT("recv:%s\n", buffer);
    }

    return 0;
}
