/*****************************************************
    > File Name: hello_client.cpp
    > Author: Yonqi
    > Mail: haidaiheshi@126.com
    > Created Time: 2016年04月27日 星期三 21时57分10秒
注释1:
 unsigned long inet_addr(const char FAR *cp );
 参数：字符串，一个点分十进制的IP地址
 返回值：如果正确执行将返回一个无符号长整数型数.如果传入的字符串不是一个合法的IP地址，将返回INADDR_NONE.
***************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char *message);
int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char message[30];
    int str_len;
    if(argc != 3){
        printf("usage: %s <IP>", argv[0]);
        exit(1);
    }
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        error_handling("bind() error");
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);  // 注释1
    serv_addr.sin_port = htons(atoi(argv[2]));
    if(connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1){
        error_handling("connect() error");
    }
    str_len = read(sock, message, sizeof(message)-1);
    if(str_len == -1){
        error_handling("read() error");
    }
    printf("message from server: %s\n", message);
    close(sock);
    return 0;
}
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
