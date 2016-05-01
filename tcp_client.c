/*****************************************************
    > File Name: hello_client.cpp
    > Author: Yonqi
    > Mail: haidaiheshi@126.com
    > Created Time: 2016年04月27日 星期三 21时57分10秒
    这是一个网络计算器的客户端
注释1:
 unsigned long inet_addr(const char FAR *cp );
 参数：字符串，一个点分十进制的IP地址
 返回值：如果正确执行将返回一个无符号长整数型数.如果传入的字符串不是一个合法的IP地址，将返回INADDR_NONE.
注释2:
 int connect(int s, const struct sockaddr * name, int namelen);
 发起连接请求,成功的话返回0,否则返回-1
 s：标识一个未连接socket
 name：指向要连接套接字的sockaddr结构体的指针
 namelen：sockaddr结构体的字节长度
注释3:
 fputs和puts的区别
 int puts(const char *string);向屏幕输出字符串并换行,它只能输出字符串,功能和printf("%s\n",s)相同
 int fputs(const char *s, FILE *stream);想指定的steam输出,如果想要输出在屏幕上面,第二个参数为stdout;
注释4:
 write有两种用法。一种是：
 ssize_t write(int handle, void *buf, int nbyte);
 handle: 是文件描述符；
 buf:    是指定的缓冲区，即指针，指向一段内存单元；
 nbyte:  是要写入文件指定的字节数；返回值：写入文档的字节数（成功）；-1（出错）
 write函数把buf中nbyte写入文件描述符handle所指的文档，成功时返回写的字节数，错误时返回-1.
 另一种是：write（const char* str,int n)
 str是字符指针或字符数组，用来存放一个字符串。n是int型数，它用来表示输出显示字符串中字符的个数。
 write（"string",strlen("string");表示输出字符串常量
注释5:
 ssize_t read[1]  (int fd, void *buf, size_t count);
 read()会把参数fd所指的文件传送nbyte个字节到buf指针所指的内存中.
 若参数nbyte为0，则read()不会有作用并返回.
 返回值为实际读取到的字节数，如果返回0，表示已到达文件尾或无可读取的数据。
 错误返回-1,并将根据不同的错误原因适当的设置错误码。
***************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4
void error_handling(char *message);
int main(int argc, char *argv[])
{
    int sock;
    int opmsg[BUF_SIZE];
    int result, opnd_cnt, i;
    struct sockaddr_in serv_addr;
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
    if(connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1){ // 注释2
        error_handling("connect() error");
    }
    else{
        puts("connected ...\n");  //注释3
    }
    fputs("operator count:", stdout);
    scanf("%d",&opnd_cnt);
    opmsg[0] = (char)opnd_cnt;
    for(i=0;i<opnd_cnt;i++){
        printf("operand%d: ", i+1);
        scanf("%d", (int*)&opmsg[i*OPSZ+1]);  // 这行什么意思
    }
    fgetc(stdin);
    fputs("operator:", stdout);
    scanf("%c", &opmsg[opnd_cnt*OPSZ+1]);
    write(sock, opmsg, opnd_cnt*OPSZ+2); // 注释4
    read(sock, &result, RLT_SIZE);   // 后两个参数前面没有进行赋值  //注释5
    printf("operator result:%d \n", result);
    close(sock);
    return 0;
}
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
