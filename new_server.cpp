#include <iostream>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
using namespace std;

int main()
{
    struct sockaddr_in s_in;//server address structure
    struct sockaddr_in c_in;//client address structure
    int l_fd,c_fd;
    socklen_t len;
    char buf[1024];//content buff area
    char addr_p[16];
    int port = 10001;
    memset((void *)&s_in,0,sizeof(s_in));

    s_in.sin_family = AF_INET;//IPV4 communication domain
    s_in.sin_addr.s_addr = INADDR_ANY;//accept any address
    s_in.sin_port = htons(port);//change port to netchar

    l_fd = socket(AF_INET,SOCK_STREAM,0);//socket(int domain, int type, int protocol)


    bind(l_fd,(struct sockaddr *)&s_in,sizeof(s_in));

    listen(l_fd,10);//lisiening start

    cout<<"begin"<<endl;
    while(1){
        for(int j=0;j<1024;j++){
            buf[j] = 0;
        }
        /*
        accept()函数让服务器接收客户的连接请求.
        accept(int sockfd, void *addr, int *addrlen);
    　　  sockfd是被监听的socket描述符，addr通常是一个指向sockaddr_in变量的指针，该变量用
        来存放提出连接请求服务的主机的信息（某台主机从某个端口发出该请求）；addrten通常为一
        个指向值为sizeof(struct sockaddr_in)的整型指针变量。出现错误时accept函数返回-1
        并置相应的errno值。
        当accept函数监视的socket收到连接请求时，socket执行体将建立一个新的socket，执行体
        将这个新socket和请求连接进程的地址联系起来
        */
        c_fd = accept(l_fd,(struct sockaddr *)&c_in,&len);

        int n = read(c_fd,buf,1024);//read the message send by client

        inet_ntop(AF_INET,&c_in.sin_addr,addr_p,16);//“二进制整数” －> “点分十进制  *inet_ntop(int af, const void *src, char *dst, socklen_t cnt);

        cout << buf;

        write(c_fd,buf,n);//sent message back to client

        close(c_fd);
    }
    cout<<buf<<endl;

    return 0;
}
