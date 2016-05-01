/*****************************************************
    > File Name: hello_server.cpp
    > Author: Yonqi
    > Mail: haidaiheshi@126.com
    > Created Time: 2016年04月27日 星期三 21时57分10秒
    这是一个网络计算器的服务端
注释1 结构体
 struct sockaddr_in
 {
    sa_famliy_t  sin_family;  一般为短整型,表示使用的地址族,AF_INET表示ipv4,AF_INET6表示ipv6
    uint16_t     sin_port;  无符号短整型,表示tcp或者udp端口号
    struct       in_addr{
                     in_addr_t s_addr;  32位整数,存储ip地址
                 };
    char         sin_zero[8];  暂时没有用
 }
 struct sockaddr{
     sa_family_t  sin_family;   //地址族（Address Family），也就是地址类型
     char         sa_data[14];  //IP地址和端口号
 };
注释2 创建套接字
 函数原型
 int socket( int domain, int type, int protocol);创建成功的话返回对套接字的描述,否则返回-1
 domain指定使用的协议族,AF_INET表示ipv4,AF_INET6表示ipv6
 type指定使用的套接字类型,SOCK_STREAM表示tcp, SOCK_DGRAM表示使用的是udp
 protocol指定使用的协议信息
注释3
 htons h可以理解为host,to即为to,n代表network,s代表short,所以它就是把short类型的主机字序转化为网络字序
 htonl l代表long,其他同上.
 网络字节统一是大端序.

注释4 绑定套接字
 函数原型
 int bind(int sock, struct sockaddr *addr, socklen_t addrlen);  //Linux 本初使用的是linux
 int bind(SOCKET sock, const struct sockaddr *addr, int addrlen);  //Windows
 sock 为 socket 文件描述符，addr 为 sockaddr 结构体变量的指针，addrlen 为 addr 变量的大小，可由 sizeof() 计算得出
 绑定的问题:
 第二个参数是结构体,为什么不用sockaddr_in,而非要转换为sockaddr?
 sockaddr 和 sockaddr_in 的长度相同，都是16字节.
 sockaddr只是将IP地址和端口号合并到一起，用一个成员 sa_data 表示,要想给 sa_data 赋值，必须同时指明IP地址和端口号，例如”127.0.0.1:80“，
 遗憾的是，没有相关函数将这个字符串转换成需要的形式，也就很难给 sockaddr 类型的变量赋值，所以使用 sockaddr_in 来代替.
 这两个结构体的长度相同，强制转换类型时不会丢失字节，也没有多余的字节。
注释5
 int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
 sockfd:   服务端套接字描述符
 addr:     保存发起连接请求的客户端地址信息,调用函数后向传来的地址变量填充客户端地址信息
 addrlen:  指向addr地址长度的整型数
***************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024
#define OPSZ 4
void error_handling(char *message);
int calculate(int opnum, int opnds[], char oprator);
int main(int argc, char *argv[])
{
    int serv_sock;
    int clnt_sock;
    char opinfo[BUF_SIZE];
    int result, opnd_cnt, i;
    int recv_cnt, recv_len;
    struct sockaddr_in serv_addr;  // 注释1
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;  // int类型
    if(argc != 2){
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);  // 注释2,创建一个套接字
    if(serv_sock == -1){
        error_handling("socket() error");
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //sin_addr应该是server internet address的简称;ｓ_addr应该是server address的简称？INADDR_ANY可以单独使用码？
    serv_addr.sin_port = htons(atoi(argv[1]));  // 注释3
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1){
        error_handling("bind() error");  // 注释4
    }
    if(listen(serv_sock, 5) == -1){  //监听创建的套接字,5表示最大的连接请求队列长度,表示可以最多有5个连接请求进入队列
        error_handling("listen() error");
    }
    clnt_addr_size = sizeof(clnt_addr);
    for(i=0;i<5;i++){
        opnd_cnt = 0;
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);  //注释5,受理请求队列中的连接请求
        read(clnt_sock, &opnd_cnt, 1);
        recv_len = 0;
        while((opnd_cnt*OPSZ+1)>recv_len){
            recv_cnt = read(clnt_sock, &opinfo[recv_len], BUF_SIZE-1);
            recv_len += recv_cnt;
        }
        result = calculate(opnd_cnt, (int*)opinfo, opinfo[recv_len-1]);
        write(clnt_sock, (char*)&result, sizeof(result));
        close(clnt_sock);
    }
    close(serv_sock);
    return 0;
}
int calculate(int opnum ,int opnds[], char op)
{
    int result = opnds[0], i;
    switch(op){
        case '+':
            for(i=1;i<opnum;i++){
                result += opnds[i];
            }
            break;
        case '-':
            for(i=1;i<opnum;i++){
                result = result - opnds[i];
            }
            break;
        case '*':
            for(i=1;i<opnum;i++){
                result *= opnds[i];
            }
            break;
    }
    return result;
}
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
