#include"RequestCode.h"
#include<cstring>
//解码构造
RequestCode::RequestCode()
{
}

//编码构造
RequestCode::RequestCode(RequestMsg* msg)
{
    memcpy(&m_msg, msg, sizeof(RequestMsg));
}

RequestCode::~RequestCode()
{

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

//编码函数
void RequestCode::encode(char** outData, int &len)
{
    Write_Header_node(m_msg.cmdType);
    Write_Next_node(m_msg.clientId, strlen(m_msg.clientId) + 1);
    Write_Next_node(m_msg.authCode, strlen(m_msg.authCode) + 1);
    Write_Next_node(m_msg.serverId, strlen(m_msg.serverId) + 1);
    Write_Next_node(m_msg.r1, strlen(m_msg.r1) + 1);
    Pack_queue(outData, len);

}

//解码函数
void* RequestCode::decode(char* inData, int len)
{
    unPack_queue(inData, len);
    memset(&m_msg, 0x00, sizeof(m_msg));
    Read_Header_node(m_msg.cmdType);
    Read_Next_node(m_msg.clientId);
    Read_Next_node(m_msg.authCode);
    Read_Next_node(m_msg.serverId);
    Read_Next_node(m_msg.r1);
    
    return &m_msg;
}


