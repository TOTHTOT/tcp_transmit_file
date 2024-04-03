/*
 * @Description: tcp 传输文件 客户端
 * @Author: TOTHTOT
 * @Date: 2024-04-01 16:12:31
 * @LastEditTime: 2024-04-03 16:54:59
 * @LastEditors: TOTHTOT
 * @FilePath: \tcp_transmit_file\client\tcp_client.c
 */
#include "tcp_client.h"

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
 * @msg:
 * @param {char} *buffer
 * @param {uint32_t} buffer_size
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-04-03 15:09:38
 */
uint8_t client_analysis_server_pack(char *buffer, uint32_t buffer_size)
{
    // 每包长度都是固定的, 如果除不尽就说明有问题
    if (buffer_size % sizeof(transmit_data_t) != 0)
        return 1; // 数据包长度错误

    uint32_t package_count = buffer_size / sizeof(transmit_data_t);
    // 分包处理
    for (uint32_t i = 0; i < package_count; i++)
    {
        transmit_data_t *transmit_data_st_p = (transmit_data_t *)(buffer + i * sizeof(transmit_data_t));
        if (transmit_data_st_p->pack_num == 0)
            INFO_PRINT("buffer_size = %d,file_name= %s\n\n", buffer_size, transmit_data_st_p->file_name);
            
        // INFO_PRINT("\nbuffer_size = %d, pack_num = %d, file_name= %s\n",
        //            buffer_size, transmit_data_st_p->pack_num, transmit_data_st_p->file_name);
        // 如果是第0包数据就删除源文件并创建新文件
        if (transmit_data_st_p->pack_num == 0)
        {
#define TEST_FILE_NAME "file.txt"
#define TEST_FILE_PATH "../linux/22"
#define TEST_FILE_FULL_PATH "../linux/22/file.txt"

            remove(TEST_FILE_FULL_PATH);
            // 创建新文件
            FILE *file = fopen(TEST_FILE_FULL_PATH, "w"); // 使用写入模式打开文件，会创建新文件或清空已存在的文件
            if (file == NULL)
            {
                ERROR_PRINT("fopen error open path = %s \n", TEST_FILE_FULL_PATH);
                perror("client_analysis_server_pack() fopen");
                return 2; // 返回 0 表示打开文件失败
            }
            // 关闭文件
            fclose(file);
        }
        // 保存数据到文件
        client_save_file(TEST_FILE_PATH, TEST_FILE_NAME, (char *)transmit_data_st_p->file_data);
    }
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
    tcp_client_info_t client_info_st = {
        .running_flag = true,
    };
    int valread = 0;
    uint8_t connect_retry_times = 0; // 连接重试次数

    // 创建套接字
    if ((client_info_st.client_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        INFO_PRINT("\n Socket creation error \n");
        return -1;
    }
    // 将套接字设置为阻塞模式
    if (fcntl(client_info_st.client_socket_fd, F_SETFL, 0) == -1)
    {
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

    // 连接服务器
    while (connect_retry_times <= 100)
    {
        // 连接到服务器
        if (connect(client_info_st.client_socket_fd, (struct sockaddr *)&client_info_st.serv_addr, sizeof(client_info_st.serv_addr)) < 0)
        {
            INFO_PRINT("Connection Failed Server offline, retry times = %d\n", connect_retry_times);
        }
        else
        {
            break;
        }
        connect_retry_times++;
        usleep(100 * 1000);
    }
    if (connect_retry_times >= 100)
    {
        INFO_PRINT("\nConnection Failed Server offline, clien exit!\n");
        return 1;
    }

    while (client_info_st.running_flag)
    {
        // 接收消息
        valread = recv(client_info_st.client_socket_fd, client_info_st.client_recv_buf, CLIENT_RECV_BUF_SIZE, 0);
        if (valread > 0)
        {
            client_analysis_server_pack(client_info_st.client_recv_buf, valread);
            memset(client_info_st.client_recv_buf, 0, CLIENT_RECV_BUF_SIZE);
        }
        else if (valread == -1)
        {
            perror("recv");
            // ERROR_PRINT("recv error\n");
        }
        usleep(100 * 1000);
    }

    return 0;
}
