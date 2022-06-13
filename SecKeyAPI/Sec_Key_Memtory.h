#pragma once
#include"Share_Memory.h"

struct NodeSHMInfo
{
	int status;
	int seckeyID;
	char clientID[12];
	char serverID[12];
	char seckey[128];
};



class Sec_Key_Memtory : public Share_Memory
{
public:
    Sec_Key_Memtory(int key);
    Sec_Key_Memtory(int key, int maxnode);
    Sec_Key_Memtory(const char* pathname);
    Sec_Key_Memtory(const char* pathname, int maxnode);
    ~Sec_Key_Memtory();

    //写共享内存
    int Write_Memory(NodeSHMInfo *info);

    //读共享内存
    int Read_Memory(const char* clientID, const char* serverID, NodeSHMInfo* info);
    int Read_Memory(int seckeyid, NodeSHMInfo* info);

    //读共享内存第一个节点
    int Read_Frist_Memory();

    //修改共享内存的的秘钥状态
    int Check_Memory(const char* clientID, const char* serverID);

    //打印共享内存的信息
    bool print_Memory();
private:
    int m_maxnode;
};


