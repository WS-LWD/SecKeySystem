#include"Client.h"
#include<iostream>
#include<cstring>
#include<openssl/hmac.h>
#include<openssl/sha.h>

using namespace std;


Client::Client(ClientInfo* info)
{
    memset(&m_info, 0x00, sizeof(ClientInfo));
    //创建共享内存
    memcpy(&m_info, info, sizeof(ClientInfo));

    m_shm = new Sec_Key_Memtory(m_info.shmKey, m_info.maxNode);
}

Client::~Client()
{
    delete m_shm;
}

void Client::SecKey_Consult()
{
    //写发送给服务端的数据  --秘钥协商
    RequestMsg msg;
    memset(&msg, 0x00, sizeof(RequestMsg));
    
    msg.cmdType = RequestCode::CONSULT;
    strcpy(msg.clientId, m_info.clinetID);
    strcpy(msg.serverId, m_info.serverID);
    getHmac(msg.r1, sizeof(msg.r1));
    char key[1024];
    unsigned char md[SHA256_DIGEST_LENGTH];
    memset(key, 0x00, sizeof(key));
    memset(md, 0x00, sizeof(md));
    unsigned int len = 0;
    sprintf(key, "@%s%s@", msg.serverId, msg.clientId);
    HMAC(EVP_sha256(), key, strlen(key), (unsigned char*)msg.r1, strlen(msg.r1), md, &len);
    for(int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        sprintf(&msg.authCode[2 * i], "%02x", md[i]);
    }
    cout << "key:" << key << endl;
	cout << "r1:" << msg.r1 << endl;
	cout << "authCode:" << msg.authCode << endl;

    //报文编码
    CodeFactory* factory = new RequestFactroy(&msg);
    Code* code = factory->createcode();
    char* outData;
    int outData_len = 0;
    code->encode(&outData, outData_len);
    

    //连接服务器
    m_socket.connectToHost(m_info.serverIP, m_info.serverPort);
    //发送随机字符串
    int ret = m_socket.sendMsg(outData, outData_len);
    if(ret != 0)
    {
        //发送数据失败
        return;
    }

    //接收服务端的随机字符串
    char* inData = nullptr;     
    int inData_len = 0;
    ret = m_socket.recvMsg(&inData, inData_len);
    if(ret != 0)
    {
        //接收数据失败
        cout << "error: recvMsg" << '\n';
        return;
    }
    
    
    delete factory;
    delete code;

    //将接收到数据解码
    factory = new Respond_Factory();
    code = factory->createcode();

    RespondMsg*respond = (RespondMsg*)code->decode(inData, inData_len);
    if(respond->rv == -1)
    {
        //日志
        exit(0);
    }
    //验证接收到数据是否被篡改
    char data[1024];
    unsigned char md_1[SHA_DIGEST_LENGTH];
    char seckey[SHA_DIGEST_LENGTH*2+1];
    memset(data, 0x00, sizeof(data));
    memset(md_1, 0x00, sizeof(md_1));
    memset(seckey, 0x00, sizeof(seckey));
    sprintf(data, "%s%s", msg.r1, respond->r2);
    SHA1((unsigned char*)data, strlen(data), md_1);
    for(int i=0; i<SHA_DIGEST_LENGTH; i++)
	{ 
		sprintf(&seckey[i*2], "%02x", md_1[i]);
	}
    printf("seckey = %s\n", seckey);

    //将秘钥写入共享内存
    NodeSHMInfo node;
    memset(&node, 0x00, sizeof(NodeSHMInfo));
    node.status = 0;
    node.seckeyID = respond->seckeyid;
    strcpy(node.clientID, respond->clientId);
    strcpy(node.serverID, respond->serverId);
    strcpy(node.seckey, seckey);

    m_shm->Write_Memory(&node);
    
    m_socket.disConnect();

    delete factory;
    delete code;
}

void Client::SecKey_Check()
{
    RequestMsg msg;
    NodeSHMInfo shminfo;
    memset(&msg, 0x00, sizeof(RequestMsg));
    memset(&shminfo, 0x00, sizeof(NodeSHMInfo));

    //读取共享内存的秘钥进行校验
    m_shm->Read_Memory(m_info.clinetID, m_info.serverID, &shminfo);

    //赋值msg
    msg.cmdType = 1;
    strcpy(msg.clientId, m_info.clinetID);
    strcpy(msg.serverId, m_info.serverID);
    strcpy(msg.r1, "bbbbb");

    //将秘钥哈希运算
    char key[1024];
    unsigned char md[SHA256_DIGEST_LENGTH];
    memset(key, 0x00, sizeof(key));
    memset(md, 0x00, sizeof(md));
    unsigned int len = 0;
    sprintf(key, "@%s%s@", msg.serverId, msg.clientId);
    HMAC(EVP_sha256(), key, strlen(key), (unsigned char*)shminfo.seckey, strlen(shminfo.seckey), md, &len);
    for(int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        sprintf(&msg.authCode[2 * i], "%02x", md[i]);
    }


    //报文编码
    CodeFactory* factory = new RequestFactroy(&msg);
    Code* code = factory->createcode();
    char* outData;
    int outData_len = 0;
    code->encode(&outData, outData_len);
    

    //连接服务器
    m_socket.connectToHost(m_info.serverIP, m_info.serverPort);
    //发送数据
    int ret = m_socket.sendMsg(outData, outData_len);

    if(ret != 0)
    {
        //发送数据失败 --日志
        return;
    }
    delete factory;
    delete code;

    //接收服务端的随机字符串
    char* inData = nullptr;     
    int inData_len = 0;
    ret = m_socket.recvMsg(&inData, inData_len);
    if(ret != 0)
    {
        //接收数据失败
        m_log.Log(__FILE__, __LINE__, ItcastLog::INFO, ret, "func recvMsg() err");
        return;
    }
    
    //将接收到数据解码
    factory = new Respond_Factory();
    code = factory->createcode();

    RespondMsg*respond = (RespondMsg*)code->decode(inData, inData_len);
    if(respond->rv != 0)
    {
        //校验失败 --日志
        m_log.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func recvMsg() err");
        exit(0);
    }
    else
    {
        m_log.Log(__FILE__, __LINE__, ItcastLog::INFO, ret, "SecKey_Check() OK");
        cout << "校验成功" << '\n';
    }
    m_socket.disConnect();
    delete factory;
    delete code;
}

void Client::SecKey_Cancel()
{
    //写发送给服务端的数据 --注销秘钥
    RequestMsg msg;
    memset(&msg, 0x00, sizeof(msg));
    msg.cmdType = RequestCode::CANCELL;
    strcpy(msg.clientId, m_info.clinetID);
    strcpy(msg.serverId, m_info.serverID);
    strcpy(msg.r1, "aaa");
    strcpy(msg.authCode, "bbbbb");

    //报文编码
    CodeFactory* factory = new RequestFactroy(&msg);
    Code* code = factory->createcode();
    char* outData;
    int outData_len = 0;
    code->encode(&outData, outData_len);
    
    //连接服务器
    m_socket.connectToHost(m_info.serverIP, m_info.serverPort);
    //发送随机字符串
    int ret = m_socket.sendMsg(outData, outData_len);
    if(ret != 0)
    {
        //发送数据失败
        cout << "error: sendMsg" << '\n';
        return;
    }
    cout << "sendMsg OK" << '\n';
    
    //接收服务端的随机字符串
    char* inData = nullptr;     
    int inData_len = 0;
    ret = m_socket.recvMsg(&inData, inData_len);
    if(ret != 0)
    {
        //接收数据失败
        m_log.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "func recvMsg() err");
        return;
    }
    else
    {
        m_log.Log(__FILE__, __LINE__, ItcastLog::INFO, ret, "func recvMsg() OK");
    }

    //解码
    factory = new Respond_Factory();
    code = factory->createcode();
    RespondMsg*respond = (RespondMsg*)code->decode(inData, inData_len);
    if(respond->rv != 0)
    {
        m_log.Log(__FILE__, __LINE__, ItcastLog::ERROR, ret, "SecKey_Cancel err");
    }
    {
        m_log.Log(__FILE__, __LINE__, ItcastLog::INFO, ret, "SecKey_Cancel OK");
    }
    m_socket.disConnect();
    
    delete factory;
    delete code;
}

void Client::Seckey_Before()
{

}

void Client::getHmac(char* str, int len)
{
    
    char s[64] = "123456789~!@#$%^&*()[]';:/.,<>";
    int s_len = strlen(s);
    for(int i = 0; i < len - 1; ++i)
    {
        int index = rand() % 3;
        switch (index)
        {
        case 0:
            str[i] = s[rand() % s_len];
            break;
        case 1:
            str[i] = rand() % 26 + 'a';
            break;
        case 2:
            str[i] = rand() % 26 + 'A';
            break;
        default:
            break;
        }
    }
    str[len - 1] = '\0';
}