/*****************************************************
    > File Name: udp_server.c
    > Author: Yonqi
    > Mail: haidaiheshi@126.com
    > Created Time: 2016年05月01日 星期日 14时56分46秒
    本文件是一个udp套接字的服务端
注释1
 int recvfrom(int sock, void *buff, size_t nbytes, int flags, struct sockaddr * from, socklen_t *addrlen);
 本函数用于从（已连接）套接口上接收数据，并捕获数据发送源的地址
 sock:    标识一个已连接套接口的描述字
 buff:    接受缓冲区的地址
 nbytes:  可以接受的最大字节数,也就是缓冲区的长度
 flags:   可选参数,没有的话即为0
 from:    存有发送端地址信息的sockaddr结构题变量的地址值,即指针
 formlen: 保存from结构体变量长度的变量地址值,即指针,指向form的长度
注释2
 ssize_t sendto(int sock, void *buff, size_t nbytes, int flags, struct sockaddr * from, socklen_t *addrlen);
 向指定目的地址发送数据
 sock:    传输数据的套接字的描述
 buff:    接受缓冲区的地址
 nbytes:  可以接受的最大字节数,也就是缓冲区的长度
 flags:   可选参数,没有的话即为0
 to:      存有目标地址信息的sockaddr结构体变量的地址值
 addrlen: 传递给参数to的地址值结构体变量长度
***************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 30

int main(int argc, char *argv[])
{
    int serv_sock;
    char message[BUF_SIZE];
    ssize_t str_len;
    socklen_t clnt_adr_sz;
    struct sockaddr_in serv_adr, clnt_adr;
    if(argc != 2){
        printf("usage: %s <port>\n", argv[0]);
        exit(1);
    }
    serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(serv_sock == -1){
        puts("sock() error");
    }
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));
    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1){
        puts("bind() error");
    }
    while(1){
        clnt_adr_sz = sizeof(clnt_adr);
        str_len = recvfrom(serv_sock, message, BUF_SIZE, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);  //注释1
        sendto(serv_sock, message, str_len, 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);  //注释2
    }
//    close(serv_sock);
//    return 0;
}
