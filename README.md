<!--
 * @Description: 基于tcp协议的文件传输项目
 * @Author: TOTHTOT
 * @Date: 2024-04-01 16:10:24
 * @LastEditTime: 2024-04-03 17:59:38
 * @LastEditors: TOTHTOT
 * @FilePath: \tcp_transmit_file\README.md
-->

# 基于tcp协议的文件传输项目

**使用 tcp/ip 协议传输文件, 绕过加密系统对对文件的保护, 实现源码同步更新.**

## 具体需求

- 程序分为 server 和 client 两部分, wsl 运行 server, Ubuntu 运行 client.

- server 负责监听终端输入: 
  
  - 当传入 "put" 命令时, 上传指定文件内最近修改的文件(数量通过 SERVER_UPLOAD_RECENT_FILE_NUM 指定)到 client;
  
  - 当传入 "put all" 命令时, 将整个项目发送到 client, 并附上 整个是整个项目的表示, 可以通过 tar 压缩后发送;
  
  - **发送的数据包含文件名称以及文件数据**;

- client 接收到 server 发送的文件后, 根据文件名保存文件到指定目录, 一般是 emb_app 文件夹下, 所有文件接收完成后, 调用 yyh_build.sh 编译工程;

## 实现功能
1. [ ] client 第一次连接时, server 要发送配置信息给 client, 包含: 每次传输文件数据最大值, 监听的目录;
2. [ ] server 配置成功监听文件功能时输出监听文件夹内文件数量;
3. [ ] server 需要校验文件夹是否存在;
4. [ ] client 接收到 server 发送的退出命令时也退出;
5. [ ] client 接收到 server 发送的文件时, 保存文件到指定目录;
6. [ ] client 掉线 server 要设置对应状态;
## 未解决bug
1. [x] 监听文件修改功能不能同时处理多个文件, 原来没有正确处理 read(server_info_st_p->file_listen_st.inotify_fd[j], buf, FILE_LISTEN_BUF_LEN) 返回的长度, 会返回多个修改过的文件信息;
2. [x] 发送完文件 pack_num 不清0, 用 calloc() 分配内存保证每次都是 0;