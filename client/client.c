/*
 * @Description: tcp 传输文件 客户端
 * @Author: TOTHTOT
 * @Date: 2024-04-01 16:12:31
 * @LastEditTime: 2024-04-01 16:46:26
 * @LastEditors: TOTHTOT
 * @FilePath: \tcp_transmit_file\client\client.c
 */
#include "../common/common.h"

int main(int argc, char *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    const char *hello = "Hello from client";

    // 创建套接字
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(TCP_USE_PORT);

    // 将 IPv4 地址从文本转换为二进制
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // 连接到服务器
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    // 接收消息
    valread = read(sock, buffer, BUFFER_SIZE);
    printf("%s\n", buffer);
    return 0;
}
