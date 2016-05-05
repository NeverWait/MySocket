/*****************************************************
    > File Name: mult_proc_server.c
    > Author: Yonqi
    > Mail: haidaiheshi@126.com
    > Created Time: 2016年05月02日 星期日 15时56分46秒
服务多个client的回声系统
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
