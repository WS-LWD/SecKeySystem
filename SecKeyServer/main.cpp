#include <iostream>
#include"threadpool.h"
#include"Jsoncpp.h"
using namespace std;

const int MAX_FD = 65536;
const int MAX_EVENT_NUMBER = 10000;



extern void addfd(int epollfd, int fd, bool one_shot);
void addsig(int sig, void(handler)(int)){ // handler回调函数，用来处理信号
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler =handler;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}

//创建为守护进程
void Daemon()
{
    pid_t pid;
    pid = fork();
    if(pid < 0 || pid >0)
    {
        exit(0);
    }
    setsid();
    chdir("/home");
    umask(0022);
    int devfd = open("/dev/null", O_RDWR);
	dup2(devfd, 0);
	// dup2(devfd, 1); // 调试的时候需要看输出
	dup2(devfd, 2);
}

int main()
{
    //从配置文件获取服务端的信息
    ServerInfo info;
    Jsoncpp json;
    json.ReadJosn(&info);

    TcpServer Socket;
    Socket.setListen((unsigned short)info.serverPORT);
    
    addsig(SIGPIPE, SIG_IGN);

    threadpool<Sec_Key_Server>*pool = nullptr;
    try
    {
        pool = new threadpool<Sec_Key_Server>;
    }
    catch(...)
    {
        return 0;
    }
    Sec_Key_Server::shmkey = info.shmkry;
    cout << "shmkey = " << Sec_Key_Server::shmkey << '\n'; 
    Sec_Key_Server::maxnode = info.MaxNode;
    Sec_Key_Memtory Shm(Sec_Key_Server::shmkey, Sec_Key_Server::maxnode);
    
    Mysql_database* mysql = new Mysql_database;
    bool is_conn = mysql->connectDB(info.serverIP, info.mysql_Use, info.mysql_Passwd, info.mysql_db, info.mysql_prot);
    if(!is_conn)
    {
        //日志, 连接失败
        cout << "连接失败" << '\n';
        exit(0);
    }
    Sec_Key_Server::m_Mysql = mysql;
    Sec_Key_Server* user = new Sec_Key_Server[MAX_FD];

    int epollfd = epoll_create(128);

    Sec_Key_Server::m_epollfd = epollfd;          //将epoll的根设置为静态

    int listenfd = Socket.get_mlfd();           //获取网络套接字

    epoll_event temp, events[MAX_EVENT_NUMBER];
    addfd(epollfd, listenfd, false);
    
    while(true)
    {
        
        int fd_num = epoll_wait(epollfd, events, 10000, -1);
        cout << "fd_num = " << fd_num << '\n';
        if(fd_num == -1)
        {
            fprintf(stderr, "epoll_wait error:%s\n", strerror(errno));
            exit(1);
        }
        for(int i = 0; i < fd_num; i++)
        {   
            int sockfd = events[i].data.fd;
            
            if(sockfd == listenfd)
            {
                
                int client_fd = Socket.acceptConn(3);
                cout << "client_fd = " << client_fd << '\n';
                struct sockaddr_in client_addr = Socket.get_client_addr();
                
                if(client_fd == -1)
                {
                    fprintf(stderr, "accept error:%s\n", strerror(errno));
                    exit(1);
                }
                if(Sec_Key_Server::m_customer_count >= MAX_FD)
                {
                    close(client_fd);
                    continue;
                }
                user[client_fd].init(client_fd, client_addr, &info);
            }
            else if(events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                user[sockfd].close_connect();
            }
            else if(events[i].events & EPOLLIN)
            {
                //user[sockfd].read_data();
                printf("-----------------read sockfd = %d---------------------\n", sockfd);
                if(user[sockfd].read_data())
                {
                   pool->append(&user[sockfd]);
                }
                else
                {
                    user[sockfd].close_connect();
                }
            }
            else if(events[i].events & EPOLLOUT)
            {
                //没实现
                printf("-----------------write sockfd = %d---------------------\n", sockfd);
                pool->append(&user[sockfd]);
            }
        }
    }
    close(epollfd);
    close(listenfd);
    delete[] user;
    delete pool;
    delete mysql;
    return 0;
}