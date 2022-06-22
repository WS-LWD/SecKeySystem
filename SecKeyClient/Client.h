#pragma once
#include"Respond_Factory.h"
#include"RequestFactroy.h"
#include"TcpSocket.h"
#include"Sec_Key_Memtory.h"

struct ClientInfo
{
	char clinetID[12];			// 客户端ID 
	char serverID[12];			// 服务器ID
	char serverIP[32];			// 服务器IP
	unsigned short serverPort;	// 服务器端口
	int maxNode;				// 共享内存节点个数
	char shmKey[32];			// 共享内存的Key
};


class Client
{

public:
    //创建共享内存
    Client(ClientInfo* info);

    ~Client();

    //秘钥协商
    void SecKey_Consult();

    //秘钥校验
    void SecKey_Check();

    //秘钥注销
    void SecKey_Cancel();

    //秘钥查看  --查看历史版本
    void Seckey_Before();
private:
    //获取随机字符串
    void getHmac(char* str, int len);
private:
    Sec_Key_Memtory* m_shm;
    TcpSocket m_socket;
    ClientInfo m_info;
    ItcastLog m_log;
};
