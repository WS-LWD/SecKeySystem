#pragma once
#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
const char randX = 'X';

class Share_Memory
{

public:
    //创建共享内存
    Share_Memory(int key);
    Share_Memory(int key, int len);
    Share_Memory(const char* pathname);
    Share_Memory(const char* pathname, int len);

    virtual ~Share_Memory();

    //连接共享内存
    void* Memory_conn();
    
    //断开连接
    int Memory_disconn();

    //删除共享内存
    int del_Mentory();

    bool is_Memtory();
private:
    int m_shmID;
    void* m_addr = nullptr;
    bool m_falg = true;  //判断共享内存是否存在

private:
    int get_shmID(key_t key, int shmsize, int flag);

    
};


