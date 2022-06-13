#include"Respond_Code.h"

Respond_Code::Respond_Code()
{
}

Respond_Code::Respond_Code(RespondMsg* msg)
{
    memset(&m_msg, 0x00, sizeof(RespondMsg));
    memcpy(&m_msg, msg, sizeof(RespondMsg));
}

Respond_Code::~Respond_Code()
{
}

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


void Respond_Code::encode(char** outData, int &len)
{
    Write_Header_node(m_msg.rv);
    Write_Next_node(m_msg.clientId, sizeof(m_msg.clientId));
    Write_Next_node(m_msg.serverId, sizeof(m_msg.serverId));
    Write_Next_node(m_msg.r2, sizeof(m_msg.r2));
    Write_Next_node(m_msg.seckeyid);

    Pack_queue(outData, len);
}

void* Respond_Code::decode(char* inData, int len)
{
    memset(&m_msg, 0x00, sizeof(RespondMsg));
    unPack_queue(inData, len);
    Read_Header_node(m_msg.rv);
    Read_Next_node(m_msg.clientId);
    Read_Next_node(m_msg.serverId);
    Read_Next_node(m_msg.r2);
    Read_Next_node(m_msg.seckeyid);

    return &m_msg;
}
