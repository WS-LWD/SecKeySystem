#pragma once
#include"Code.h"

struct RequestMsg
{
    int cmdType; 	   //0-秘钥协商 1-秘钥校验 2-秘钥注销 3-秘钥查看
    char clientId[12]; //客户端ID, 是唯一的, 不同的客户端有不同的ID
    char authCode[65]; //消息验证码, 对r1进行验证
    char serverId[12]; //服务端ID
    char r1[64];       //客户端生成的随机字符串
};

class RequestCode : public Code
{
public:
    /*CONSULT:秘钥协商  CHECK:秘钥校验  CANCELL:秘钥注销  BEFORE_VIEW:查看之前的秘钥*/
    enum {CONSULT = 0, CHECK, CANCELL, BEFORE_VIEW};

public:
    RequestCode();
    RequestCode(RequestMsg* msg);
    ~RequestCode();

    virtual void encode(char** outData, int &len);
    virtual void* decode(char* inData, int len);

private:
    RequestMsg m_msg;
};
