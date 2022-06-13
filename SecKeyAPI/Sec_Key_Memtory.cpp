#include"Sec_Key_Memtory.h"
#include<iostream>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<cstring>

using namespace std;

Sec_Key_Memtory::Sec_Key_Memtory(int key):Share_Memory(key)
{

}

Sec_Key_Memtory::Sec_Key_Memtory(int key, int maxnode):Share_Memory(key, maxnode*sizeof(NodeSHMInfo) + sizeof(int)),m_maxnode(maxnode)
{
    void* addr = Memory_conn();     //连接共享内存

    memcpy(addr, &m_maxnode, sizeof(int));

    NodeSHMInfo* pnode = (NodeSHMInfo*)((char*)addr + sizeof(int));
    if(is_Memtory())
    {
        memset(pnode, 0x00, maxnode*sizeof(NodeSHMInfo));
    }
    Memory_disconn();           //断开连接

}   

Sec_Key_Memtory::Sec_Key_Memtory(const char* pathname):Share_Memory(pathname)
{
    
}

Sec_Key_Memtory::Sec_Key_Memtory(const char* pathname, int maxnode):Share_Memory(pathname, maxnode*sizeof(NodeSHMInfo) + sizeof(int)),m_maxnode(maxnode)
{
    void* addr = Memory_conn();
    memcpy(addr, &m_maxnode, sizeof(int));

    NodeSHMInfo* pnode = (NodeSHMInfo*)((char*)addr + sizeof(int));
    if(is_Memtory())
    {
        memset(pnode, 0x00, maxnode*sizeof(NodeSHMInfo));
    }

    Memory_disconn();           //断开连接
}

Sec_Key_Memtory::~Sec_Key_Memtory()
{
}

int Sec_Key_Memtory::Write_Memory(NodeSHMInfo* info)
{
    //连接共享内存
    void* addr_ptr = Memory_conn();

    //获取共享内存的大小,前四个字节表示大小
    memcpy(&m_maxnode, addr_ptr, sizeof(int));

    NodeSHMInfo* node_ptr = (NodeSHMInfo*)((char*)addr_ptr + sizeof(int));


    int i = 0;
    for(i = 0; i < m_maxnode; ++i)
    {
        if(strcmp(node_ptr[i].clientID, info->clientID) == 0 && strcmp(node_ptr[i].serverID, info->serverID) == 0)
        {
            memcpy(&node_ptr[i], info, sizeof(NodeSHMInfo));
            break;
        }
    }

    NodeSHMInfo temp_info;
    memset(&temp_info, 0x00, sizeof(temp_info));
    if(i == m_maxnode)
    {
        for(i = 0; i < m_maxnode; ++i)
        {
            if(memcmp(&node_ptr[i], &temp_info, sizeof(NodeSHMInfo)) == 0)
            {
                memcpy(&node_ptr[i], info, sizeof(NodeSHMInfo));
                break;
            }
        }
    }

    //没有可用内存
    if(i == m_maxnode)
    {
        //打印日志
        Memory_disconn();
        return -1;
    }

    Memory_disconn();
    return 0;
}

int Sec_Key_Memtory::Read_Memory(const char* clientID, const char* serverID, NodeSHMInfo* info)
{
    //连接共享内存
    void* addr_ptr = Memory_conn();

    //获取共享内存的大小,前四个字节表示大小
    memcpy(&m_maxnode, addr_ptr, sizeof(int));

    NodeSHMInfo* node_ptr = (NodeSHMInfo*)((char*)addr_ptr + sizeof(int));

    int i = 0;
	for(i = 0; i < m_maxnode; ++i)
	{
		if(strcmp(node_ptr[i].clientID, clientID)==0 && strcmp(node_ptr[i].serverID, serverID)==0)
		{
			//将秘钥信息写入到这个位置
			memcpy(info, &node_ptr[i], sizeof(NodeSHMInfo));
			break;
		}
	}

    //没有该记录
    if(i == m_maxnode)
    {
        //打印日志
        Memory_disconn();
        return -1;
    }

    Memory_disconn();
    return 0;
}

int Sec_Key_Memtory::Read_Memory(int seckeyid, NodeSHMInfo* info)
{
    //连接共享内存
    void* addr_ptr = Memory_conn();

    //获取共享内存的大小,前四个字节表示大小
    memcpy(&m_maxnode, addr_ptr, sizeof(int));

    NodeSHMInfo* node_ptr = (NodeSHMInfo*)((char*)addr_ptr + sizeof(int));

    int i = 0;
	for(i = 0; i < m_maxnode; ++i)
	{
		if(node_ptr[i].seckeyID == seckeyid)
		{
			//将秘钥信息写入到这个位置
			memcpy(info, &node_ptr[i], sizeof(NodeSHMInfo));
			break;
		}
	}

    //没有该记录
    if(i == m_maxnode)
    {
        //打印日志
        Memory_disconn();
        return -1;
    }

    Memory_disconn();
    return 0;
}

int Sec_Key_Memtory::Read_Frist_Memory()
{
    //连接共享内存
    void* addr_ptr = Memory_conn();

    //获取共享内存的大小,前四个字节表示大小
    memcpy(&m_maxnode, addr_ptr, sizeof(int));

    NodeSHMInfo* node_ptr = (NodeSHMInfo*)((char*)addr_ptr + sizeof(int));
    int shmkeyid = node_ptr[0].seckeyID;

    Memory_disconn();
    return shmkeyid;
}

int Sec_Key_Memtory::Check_Memory(const char* clientID, const char* serverID)
{
    void* addr_ptr = Memory_conn();

    //获取共享内存的大小,前四个字节表示大小
    memcpy(&m_maxnode, addr_ptr, sizeof(int));

    NodeSHMInfo* node_ptr = (NodeSHMInfo*)((char*)addr_ptr + sizeof(int));

    int i = 0;
	for(i = 0; i < m_maxnode; ++i)
	{
		if(strcmp(node_ptr[i].clientID, clientID)==0 && strcmp(node_ptr[i].serverID, serverID)==0)
		{
			//将此秘钥设为不可用
            node_ptr[i].status = 1;
			break;
		}
	}

    //没有该记录
    if(i == m_maxnode)
    {
        //打印日志
        cout << "没有该记录 i = " << i<< '\n';
        Memory_disconn();
        return -1;
    }

    Memory_disconn();
    return 0;
}

bool Sec_Key_Memtory::print_Memory()
{
    //连接共享内存
    void* addr_ptr = Memory_conn();

    //获取共享内存的大小,前四个字节表示大小
    memcpy(&m_maxnode, addr_ptr, sizeof(int));
    cout << "m_maxnode = "<< m_maxnode << '\n';

    NodeSHMInfo* node_ptr = (NodeSHMInfo*)((char*)addr_ptr + sizeof(int));
    for(int i=0; i<m_maxnode; i++)
	{
		cout << "--------" << i << "-------" << '\n';
		cout << "status:" <<node_ptr[i].status << '\n';
		cout << "seckeyID:" << node_ptr[i].seckeyID << '\n';
		cout << "clientID:" << node_ptr[i].clientID << '\n';
		cout << "serverID:" << node_ptr[i].serverID << '\n';
		cout << "seckey:" << node_ptr[i].seckey << '\n';
		cout << "--------------------" << '\n';
	}
    Memory_disconn();

    return true;
}
