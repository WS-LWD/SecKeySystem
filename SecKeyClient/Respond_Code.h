#pragma once
#include"Code.h"
struct RespondMsg
{
    int rv; 		   //0-表示成功 -1-表示失败
    char clientId[12]; //客户端ID
    char serverId[12]; //服务端ID 
    char r2[64]; 	   //秘钥协商: 随机字符串, 秘钥查看: 秘钥  秘钥校验和注销:可不写 
    int seckeyid; 	   //秘钥ID   
};

class Respond_Code : public Code
{

public:
    
    //解码构造
    Respond_Code();
    
    //编码构造
    Respond_Code(RespondMsg* msg);

    //编码
    virtual void encode(char** outData, int &len);

    //解码
    virtual void* decode(char* inData, int len);

    ~Respond_Code();
private:
    RespondMsg m_msg;
};



