/*****************************************************
    > File Name: mult_proc_server.c
    > Author: Yonqi
    > Mail: haidaiheshi@126.com
    > Created Time: 2016年05月02日 星期日 15时56分46秒
服务多个client的回声系统,由管道传递信息,并将客户端发的信息写到文件中,
使用到的知识点:
信号机制,多进程机制,socket,管道通信
***************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <wait.h>
#define BUF_SIZE 30

void read_child_proc(int sig)
{
    int status;
    pid_t id = waitpid(-1, &status, WNOHANG);
    if(WIFEXITED(status)){    //WIFEXITED(status)这个宏用来指出子进程是否为正常退出的，如果是，它会返回一个非零值
        printf("removed proc id: %d\n", id);
    }
}

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;;
    struct sockaddr_in serv_addr, clnt_addr;
    int fds[2];

    pid_t pid;
    struct sigaction act;
    socklen_t adr_sz;
    int str_len, state;
    char buf[BUF_SIZE];
    if(argc!=2){
        printf("usage: %s <port>\n", argv[0]);
        exit(1);
    }
    act.sa_handler = read_child_proc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD, &act, 0);

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1){
        printf("bind error\n");
    }
    if(listen(serv_sock, 5)==-1){
        printf("listen error\n");
    }
    pipe(fds);
    pid = fork();

    if(pid==0){
        FILE *fp = fopen("echomsg.txt", "wt");
        char msgbuf[BUF_SIZE];
        int i, len;
        for(i=0;i<10;i++){
            len = read(fds[0], msgbuf, BUF_SIZE);
            fwrite((void*)msgbuf, 1, len, fp);
        }
        fclose(fp);
        return 0;
    }

    while(1){
        adr_sz = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &adr_sz);
        if(clnt_sock==-1){
            continue;
        }
        else{
            printf("new client connected...\n");
        }
        pid = fork();
        if(pid==0){
            close(serv_sock);
            while((str_len=read(clnt_sock, buf, BUF_SIZE))!=0){
                write(clnt_sock, buf, str_len);
                write(fds[1], buf, str_len);
            }
            close(clnt_sock);
            printf("client disconnected...\n");
            return 0;
        }
        else{
            close(clnt_sock);
        }
    }
}

//下面是client.c,一个服务端运行可以同时服务多个client
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024
void error_handling(char *message);
int main(int argc, char *argv[])
{
    int sock;
    char message[BUF_SIZE];
    int str_len;
    struct sockaddr_in serv_addr;
    if(argc!=3){
        printf("usage:%s <IP> <port>\n", argv[0]);
        exit(1);
    }
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock==-1){
        error_handling("socket() error\n");
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1){
        error_handling("connect error\n");
    }
    else{
        printf("connected ...\n");
    }
    while(1){
        printf("input message(q/Q to quit):\n");
        fgets(message, BUF_SIZE, stdin);
        if(!strcmp(message, "q\n") || !strcmp(message, "Q\n")){
            break;
        }
        write(sock, message, strlen(message));
        str_len = read(sock, message, BUF_SIZE-1);
        message[str_len-1] = 0;
        printf("message from server is: %s\n", message);
    }
    close(sock);
    return 0;
}
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

//这个和上面的client.c二选一,这个是实现了读写分离,功能是一样的
/*****************************************************
    > File Name: echo_myclient.c
    > Author: Yonqi
    > Mail: haidaiheshi@126.com
    > Created Time: 2016年05月02日 星期日 15时56分46秒
注释1
***************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <wait.h>
#define BUF_SIZE 30

void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);
int main(int argc, char *argv[])
{
    int sock;
    pid_t pid;
    char buf[BUF_SIZE];
    struct sockaddr_in serv_adr;
    if(argc!=3){
        printf("usage: %s <ip> <port>\n", argv[0]);
    }
    sock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));
    if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1){
        printf("connect error");
    }
    pid = fork();
    if(pid==0){
        write_routine(sock, buf);
    }
    else{
        read_routine(sock, buf);
    }
    close(sock);
    return 0;
}
void read_routine(int sock, char *buf)
{
    while(1){
        int str_len = read(sock, buf, BUF_SIZE);
        if(str_len==0){
            return;
        }
        buf[str_len] = 0;
        printf("message from serve is:%s\n", buf);
    }
}
void write_routine(int sock, char *buf)
{
    while(1){
        fgets(buf, BUF_SIZE, stdin);
        if(!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")){
            shutdown(sock, SHUT_WR);
            return;
        }
        write(sock, buf, strlen(buf));
    }
}
