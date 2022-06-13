#pragma once
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <cstring>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/uio.h>
#include"Respond_Factory.h"
#include"RequestFactroy.h"
#include"Sec_Key_Memtory.h"
#include"TcpServer.h"
#include"Mysql_database.h"

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

/*
struct NodeSHMInfo
{
	int status;
	int seckeyID;
	char clientID[12];
	char serverID[12];
	char seckey[128];
};
*/

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
/*
class ServerInfo
{
public:
	char			serverID[12];	// 服务器端编号
	char			dbUse[24]; 		// 数据库用户名
	char			dbPasswd[24]; 	// 数据库密码
	char			dbSID[24]; 		// 数据库sid

	unsigned short 	sPort;			// 服务器绑定的端口
	int				maxnode;		// 共享内存最大网点树 客户端默认1个
	int 			shmkey;			// 共享内存keyid 创建共享内存时使用	 
};
*/


struct ServerInfo       //服务端的基本信息
{
    char shmkry[32];
    char serverIP[32];        //服务端IP
    int serverPORT;      //网络端口
	char serverID[32];	    // 服务器端编号
    int MaxNode;
	char mysql_Use[32]; 		    // 数据库用户名
	char mysql_db[32]; 		// 数据库名
    char mysql_Passwd[32]; 	    // 数据库密码
	int mysql_prot; 		// 数据库端口
};


class Sec_Key_Server
{
public:
    /*线程读写状态的2种情况
    READ:   当前线程处于读状态  /读数据
    WRITE:  当前线程处于写状态  /响应数据
    */
    enum THREAD_RDWR {M_READ = 0, M_WRIET};

public:
    static int m_epollfd;
    static int m_customer_count;
    static char* shmkey;                  // 共享内存keyid 创建共享内存时使用
    static int maxnode;                 // 共享内存最大网点树 客户端默认1个
    static Mysql_database* m_Mysql;
    

public:
    Sec_Key_Server(){}
    ~Sec_Key_Server(){}

    void init(int fd, const sockaddr_in& client_addr, ServerInfo *info);      //
    
    //初始化成员变量
    void init_data();

    //获取数据
    bool read_data();
    
    //解析收到的数据
    void resolution();

    void process();

    //验证 ---是否有人篡改过数据
    bool Check_isData(RequestMsg* m_request);

    //秘钥协商
    void Sec_Key_Consult(RequestMsg* m_request);
    
    
    //秘钥校验
    void Sec_key_Check(RequestMsg* m_request);

    //秘钥注销
    void Sec_key_Cancell(RequestMsg* m_request);

    //秘钥查看 --历史版本
    void Sec_key_View();

    //断开连接
    void close_connect();

private:
    void getHmac(char* str, int len);

private:
    int m_sockfd;                                           //通信套接字
    sockaddr_in m_addr;                                     //
    TcpSocket tcpSocket;                                    //tcp通信类的对象 --读写
    ServerInfo m_info;                                      //服务端的基本信息
    NodeSHMInfo m_shm_info;                                  //共享内存结构体对象
    RespondMsg m_respond_info;
    RequestMsg m_reques_info;

    THREAD_RDWR thread_rdwr;                                //线程读写状态
    char *Data_code;                                        //编码,解码之后保存的数据
    int Data_code_len;                                      //outData的数据长度
    ItcastLog m_log;
    
};

