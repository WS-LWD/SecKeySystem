#include"Share_Memory.h"

#include<errno.h>
#include<iostream>

using namespace std;

int Share_Memory::get_shmID(key_t key, int shmsize, int flag)
{
    int shmid = shmget(key, shmsize, flag);
    if(shmid < 0)
    {
	    if(errno == EEXIST)
	    {
		    shmid = shmget(key, 0, 0); 
            m_falg = false;
		    return shmid;
	    }
    }
    return shmid;
}

Share_Memory::Share_Memory(int key)
{
    m_shmID = get_shmID(key, 0, 0);
}

Share_Memory::Share_Memory(int key, int len)
{
    m_shmID = get_shmID(key, len, IPC_CREAT|IPC_EXCL|0755);
}

Share_Memory::Share_Memory(const char* pathname)
{
    key_t key = ftok(pathname, randX);
    m_shmID = get_shmID(key, 0, 0);
}

Share_Memory::Share_Memory(const char* pathname, int len)
{
    key_t key = ftok(pathname, randX);
    m_shmID = get_shmID(key, len, IPC_CREAT|IPC_EXCL|0755);
}

Share_Memory::~Share_Memory()
{
}

void* Share_Memory::Memory_conn()
{
    m_addr = shmat(m_shmID, NULL, 0);
    if(m_addr == (void*)-1)
    {
        //打印日志
        return (void*)-1;
    }
    return m_addr;
}

int Share_Memory::Memory_disconn()
{
    int ret = shmdt(m_addr);
    if(ret == -1)
    {
        //打印日志 
        //返回-1, 并设置errno
        return -1;
    }
    return ret;
}

int Share_Memory::del_Mentory()
{
    int ret = shmctl(m_shmID, IPC_RMID, NULL);
    return ret;
}

bool Share_Memory::is_Memtory()
{
    return m_falg;
}