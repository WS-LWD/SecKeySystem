#pragma once
#include"BaseASN1.h"



class BaseASN1Code : public BaseASN1
{

public:
    BaseASN1Code(/* args */);
    ~BaseASN1Code();

    //创建头节点编码 --头部 
    int Write_Header_node(int val);
    int Write_Header_node(char* str, int len);

    //添加节点  --节点
    int Write_Next_node(int val);
    int Write_Next_node(char* str, int len);

    //以int类型作为header解码 --头部 write
    int Read_Header_node(int &val);
    int Read_Header_node(char* str);

    //读后继节点解码    --节点
    int Read_Next_node(int &val);
    int Read_Next_node(char* str);

    //编码打包
    int Pack_queue(char** outData, int &len);

    //解码链表
    int unPack_queue(char* inData, int &len);
    


private:
    ITCAST_ANYBUF* m_header = nullptr;
    ITCAST_ANYBUF* m_next = nullptr;
    ITCAST_ANYBUF* m_temp = nullptr;
};

