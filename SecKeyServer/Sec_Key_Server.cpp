#include<iostream>
#include<openssl/sha.h>
#include<openssl/hmac.h>
#include"Sec_Key_Server.h"

using namespace std;

int Sec_Key_Server::m_customer_count = 0;                     //记录客户数量
int Sec_Key_Server::m_epollfd = -1;                           //将epoll的根设置为静态
char* Sec_Key_Server::shmkey = nullptr;
int Sec_Key_Server::maxnode = -1;
Mysql_database *Sec_Key_Server::m_Mysql = nullptr;
//设置非阻塞
void setnonblockfd(int fd)                      
{
    int old_option = fcntl(fd, F_GETFL);       // 通过fcntl可以改变已打开的文件性质。fcntl针对描述符提供控制，F_GeTFL:Get file status flags.
    old_option = old_option | O_NONBLOCK;             // 加上非阻塞
    fcntl(fd, F_SETFL, old_option);             // 设置文件描述符状态标志
}

//将套接字挂上红黑树
void addfd(int epollfd, int fd, bool one_shot)  
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLRDHUP;
    if(one_shot)
    {
        event.events |= EPOLLONESHOT;   // 防止不同的线程或者进程在处理同一个SOCKET的事件
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblockfd(fd);
}

//将套接字从红黑树摘下
void remove_fd(int epollfd, int fd)            
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
    cout <<"成功关闭 fd = "<< fd << endl;
}

//修改套接字的触发条件
void modfd(int epollfd, int fd, int ev)        
{
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;// 再把EPOLLONESHOT加回来（因为已经触发过一次了）
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);        // 参数准备齐全，修改指定的epoll文件描述符上的事件（EPOLL_CTL_MOD：修改）
}

//关闭连接
void Sec_Key_Server::close_connect()
{
    remove_fd(m_epollfd, m_sockfd);
    init_data();
    memset(&m_info, 0x00, sizeof(ServerInfo));
    m_sockfd = -1;
    m_customer_count--;
}

void Sec_Key_Server::init(int fd, const sockaddr_in& client_addr, ServerInfo *info)
{
    bzero(&m_addr, sizeof(m_addr));
    m_sockfd = fd;
    m_addr = client_addr;
    int reuse=1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)); //端口复用

    memset(&m_info, 0x00, sizeof(ServerInfo));
    memcpy(&m_info, info, sizeof(ServerInfo));
    addfd(m_epollfd, m_sockfd, true);
    init_data();
    m_customer_count++;
}

void Sec_Key_Server::init_data()
{
    
    memset(&m_reques_info, 0x00, sizeof(m_reques_info));
    memset(&m_respond_info, 0x00, sizeof(m_respond_info));
    memset(&m_shm_info, 0x00, sizeof(m_shm_info));


    thread_rdwr = M_READ;
    Data_code = nullptr;
    Data_code_len = -1;
}

bool Sec_Key_Server::read_data()
{
    tcpSocket.Achieve_sock(m_sockfd);
    int ret = tcpSocket.recvMsg(&Data_code, Data_code_len);
    cout << "ret = " << ret << '\n';
    if(ret != 0)
    {
        init_data();
        close_connect();
        return false;
    }
    return true;
}

void Sec_Key_Server::resolution()
{
    //解码
    CodeFactory *factory = new RequestFactroy();
    Code *code = factory->createcode();
    RequestMsg* request = (RequestMsg*)code->decode(Data_code, Data_code_len);
        
    if(!m_Mysql->check_client(request->clientId))
    {
        cout<< "clientID error" << '\n';
        //回复客户端是什么错误原因 --没有该客户端id
        //打印日志
        return;
    }
    

    
    
    //查看请求服务
    Data_code = nullptr;
    Data_code_len = -1;
    cout << "request->cmdType" << request->cmdType << '\n';
    switch (request->cmdType)
    {
    case RequestCode::CONSULT:          //秘钥协商
        Sec_Key_Consult(request);
        break;
    case RequestCode::CHECK:            //秘钥校验
        Sec_key_Check(request);
        break;
    case RequestCode::CANCELL:          //秘钥注销
        Sec_key_Cancell(request);
        break;
    case RequestCode::BEFORE_VIEW:      //秘钥查看 --历史版本
        Sec_key_View();
        break;
    default:
        break;
    }
    //返回客户端请求
    
    delete factory;
    delete code;
}
/*
struct RespondMsg
{
    int rv; 		   //0-表示成功 -1-表示失败
    char clientId[12]; //客户端ID
    char serverId[12]; //服务端ID 
    char r2[64]; 	   //秘钥协商: 随机字符串, 秘钥查看: 秘钥  秘钥校验和注销:可不写 
    int seckeyid; 	   //秘钥ID   
};
*/

void Sec_Key_Server::Sec_Key_Consult(RequestMsg* m_request)
{

    //验证 ---是否有人篡改过数据
    //Check_isData
    bool is_data = Check_isData(m_request);
    if(!is_data)
    {
        return;
    }
    //-------------------------------------------------------------

    RespondMsg msg;
    memset(&msg, 0x00, sizeof(msg));
    msg.rv = 0;
    msg.seckeyid = (int)m_Mysql->getKeyID();
    strcpy(msg.clientId, m_request->clientId);
    strcpy(msg.serverId, m_info.serverID);
    getHmac(msg.r2, sizeof(msg.r2));
    CodeFactory * factory = new Respond_Factory(&msg);
    Code* code = factory->createcode();
    //进行编码
    code->encode(&Data_code, Data_code_len);


    char data[1024];
    unsigned char md_1[SHA_DIGEST_LENGTH];
    char seckey[SHA_DIGEST_LENGTH*2+1];
    memset(data, 0x00, sizeof(data));
    memset(md_1, 0x00, sizeof(md_1));
    memset(seckey, 0x00, sizeof(seckey));
    sprintf(data, "%s%s", m_request->r1, msg.r2);
    //生成秘钥
    SHA1((unsigned char*)data, strlen(data), md_1);
    for(int i=0; i<SHA_DIGEST_LENGTH; i++)
	{ 
		sprintf(&seckey[i*2], "%02x", md_1[i]);
	}
    printf("seckey = %s\n", seckey);

    //发送数据
    int ret = tcpSocket.sendMsg(Data_code, Data_code_len);
    if(ret != 0)
    {
        init_data();
        close_connect();
        return;
    }

    NodeSHMInfo shminfo;
    shminfo.status = 0; //0 表示可用
    shminfo.seckeyID = msg.seckeyid;
    strcpy(shminfo.clientID, msg.clientId);
    strcpy(shminfo.seckey, seckey);
    strcpy(shminfo.serverID, m_request->serverId);
    Sec_Key_Memtory m_shm(shmkey, maxnode); 
    //写入共享内存
    m_shm.Write_Memory(&shminfo);
    

    //写入数据数据库
    m_Mysql->updata_status(msg.clientId,msg.serverId);
    m_Mysql->updataKeyID(msg.seckeyid + 1);
    m_Mysql->writeSecKey(&shminfo);
    delete factory;
    delete code;
}

bool Sec_Key_Server::Check_isData(RequestMsg* m_request)
{
    char key[1024];
    char buf_data[SHA256_DIGEST_LENGTH * 2 + 1];
    unsigned char md[SHA256_DIGEST_LENGTH];
    unsigned int len = 0;

    memset(buf_data, 0x00, sizeof(buf_data));
    memset(key, 0x00, sizeof(key));
    memset(md, 0x00, sizeof(md));

    
    sprintf(key, "@%s%s@", m_request->serverId, m_request->clientId);
    HMAC(EVP_sha256(), key, strlen(key), (unsigned char*)m_request->r1, strlen(m_request->r1), md, &len);


    printf("key = %s\n", key);
    for(int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        sprintf(&buf_data[2 * i], "%02x", md[i]);
    }
    //比较验证码是否正确
    if(strcmp(buf_data, m_request->authCode) != 0)
    {
        //回复客户端信息错误信息
        
        printf("buf_data = %s\n", buf_data);
        printf("request->authCode = %s\n", m_request->authCode);
        close_connect();
        return false;
    }
    return true;
}
/*
struct RequestMsg
{
    int cmdType; 	   //0-秘钥协商 1-秘钥校验 2-秘钥注销 3-秘钥查看
    char clientId[12]; //客户端ID, 是唯一的, 不同的客户端有不同的ID
    char authCode[65]; //消息验证码, 对r1进行验证
    char serverId[12]; //服务端ID
    char r1[64];       //客户端生成的随机字符串
};
*/

void Sec_Key_Server::Sec_key_Check(RequestMsg* m_request)
{

    //验证用clientid 和 serverid 为key, 对秘钥进行哈希运算的到值作比较
    char key[1024];
    char buf_data[SHA256_DIGEST_LENGTH * 2 + 1];
    unsigned char md[SHA256_DIGEST_LENGTH];
    unsigned int len = 0;

    memset(buf_data, 0x00, sizeof(buf_data));
    memset(key, 0x00, sizeof(key));
    memset(md, 0x00, sizeof(md));
    //连接共享内存
    Sec_Key_Memtory m_shm(shmkey, maxnode);
    int ret = m_shm.Read_Memory(m_request->clientId, m_request->serverId, &m_shm_info);
    
    //回复客户端信息
    RespondMsg msg;
    memset(&msg, 0x00, sizeof(msg));

    if(ret != 0)
    {
        //打印日志,读取失败 
        m_log.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "没有该秘钥");
        //回复客户端的失败原因  --没实现
        close_connect();
        return;
    }
    
    sprintf(key, "@%s%s@", m_request->serverId, m_request->clientId);
    HMAC(EVP_sha256(), key, strlen(key), (unsigned char*)m_shm_info.seckey, strlen(m_shm_info.seckey), md, &len);

    printf("key = %s\n", key);
    for(int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        sprintf(&buf_data[2 * i], "%02x", md[i]);
    }

    if(strcmp(m_request->authCode, buf_data) != 0)
    {
        //回复客户端信息错误信息 --没实现
        m_log.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "校验失败!");
        close_connect();
        return;
    }

    Data_code =nullptr;
    msg.rv = 0;
    CodeFactory * factory = new Respond_Factory(&msg);
    Code* code = factory->createcode();
    //进行编码
    code->encode(&Data_code, Data_code_len);
    ret = tcpSocket.sendMsg(Data_code, Data_code_len);
    if(ret != 0)
    {
        init_data();
        close_connect();
        return;
    }

    delete factory;
    delete code;
}

void Sec_Key_Server::Sec_key_Cancell(RequestMsg* m_request)
{
    //连接共享内存
    Sec_Key_Memtory m_shm(shmkey);
    cout << "m_request->clientId = " << m_request->clientId << " m_request->serverId = " << m_request->serverId << endl;
    int ret = m_shm.Check_Memory(m_request->clientId, m_request->serverId);
    if(ret != 0)
    {
        //回复客户端  --注销失败
        //打印日志
        cout << "error: Check_Memory" << '\n';
        return;
    }
    cout << "Check_Memory OK" << '\n';
    bool falg = m_Mysql->updata_status(m_request->clientId, m_request->serverId);
    if(!falg)
    {
        
        
        cout << "error: read database no" << '\n';
        return;
    }

    //回复客户端  --注销成功
    RespondMsg msg;
    memset(&msg, 0x00, sizeof(msg));
    msg.rv = 1;
    CodeFactory * factory = new Respond_Factory(&msg);
    Code* code = factory->createcode();
    //进行编码
    code->encode(&Data_code, Data_code_len);
    //发送数据
    ret = tcpSocket.sendMsg(Data_code, Data_code_len);
    if(ret != 0)
    {
        init_data();
        close_connect();
        return;
    }
    delete factory;
    delete code;
    //回复客户端  --注销成功  
}

void Sec_Key_Server::Sec_key_View()
{
    
}



void Sec_Key_Server::getHmac(char* str, int len)
{
    
    char s[64] = "123456789~!@#$%^&*()[]';:/.,<>";
    int s_len = strlen(s);
    for(int i = 0; i < len - 1; ++i)
    {
        int index = rand() % 3;
        switch (index)
        {
        case 0:
            str[i] = s[rand() % s_len];
            break;
        case 1:
            str[i] = rand() % 26 + 'a';
            break;
        case 2:
            str[i] = rand() % 26 + 'A';
            break;
        default:
            break;
        }
    }
    str[len - 1] = '\0';
}

void Sec_Key_Server::process()
{
    
    switch (thread_rdwr)
    {
    case M_READ:
    {
        
        resolution();
        close_connect();
        cout << endl;
        break;
    }
    case M_WRIET:
    {
        break;
    }
    default:
        break;
    }
}

