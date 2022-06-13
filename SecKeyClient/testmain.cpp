/*
#include<iostream>
#include<cstring>

#include"RequestFactroy.h"
#include"RequestCode.h"
#include"Respond_Code.h"
#include"Respond_Factory.h"
#include"Client.h"
#include"Sec_Key_Memtory.h"

#include<openssl/hmac.h>
#include<openssl/sha.h>
#include<time.h>
using namespace std;
*/
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

/*
struct NodeSHMInfo
{
	int status;
	int seckeyID;
	char clientID[12];
	char serverID[12];
	char seckey[128];
};
*/

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
//测试报文编码
/*
int main()
{
    RespondMsg msg;
    memset(&msg, 0x00, sizeof(msg));
    msg.rv = 1;
    strcpy(msg.clientId, "0111");
    strcpy(msg.serverId, "0666566");
    strcpy(msg.r2, "123456");
    msg.seckeyid = 1;

    char *outData;
    int len = 0;
    BaseASN1Code code;
    code.Write_Header_node(msg.rv);
    code.Write_Next_node(msg.clientId, strlen(msg.clientId));
    code.Write_Next_node(msg.serverId, strlen(msg.serverId));
    code.Write_Next_node(msg.r2, strlen(msg.r2));
    code.Write_Next_node(msg.seckeyid);
    code.Pack_queue(&outData, len);


    RespondMsg msg1;
    BaseASN1Code code1;
    memset(&msg1, 0x00, sizeof(msg1));
    code1.unPack_queue(outData, len);
    code1.Read_Header_node(msg1.rv);
    printf("rv = %d\n", msg1.rv);
    code1.Read_Next_node(msg1.clientId);
    printf("clientId = %s\n", msg1.clientId);
    code1.Read_Next_node(msg1.serverId);
    printf("serverId = %s\n", msg1.serverId);
    code1.Read_Next_node(msg1.r2);
    printf("r2 = %s\n", msg1.r2);
    code1.Read_Next_node(msg1.seckeyid);
    printf("seckeyid = %d\n", msg1.seckeyid);
    
    return 0;
}
*/

//测试使用工厂模式的request报文编码
/*
int main()
{
    RequestMsg msg;
    memset(&msg, 0x00, sizeof(msg));
    msg.cmdType = 0;
    strcpy(msg.clientId, "11111");
    strcpy(msg.authCode, "fsdhfsd");
    strcpy(msg.serverId, "00001");
    strcpy(msg.r1, "lwdyyds");
    CodeFactory * factory = new RequestFactroy(&msg);
    Code* code = factory->createcode();
    char* outData;
    int len = 0;
    //进行编码
    code->encode(&outData, len);
    delete factory;
    delete code;

    factory = new RequestFactroy();
    code = factory->createcode();

    RequestMsg* data = (RequestMsg*)code->decode(outData, len);

    cout << "cmdType = " << data->cmdType << '\n';
    cout << "clientId = " << data->clientId << '\n';
    cout << "authCode = " << data->authCode << '\n';
    cout << "serverId = " << data->serverId << '\n';
    cout << "r1 = " << data->r1 << '\n';
    return 0;

}
*/

//测试使用工厂模式的respond报文编码
/*
int main()
{
    RespondMsg msg;
    memset(&msg, 0x00, sizeof(msg));
    msg.rv = 0;
    msg.seckeyid = 1;
    strcpy(msg.clientId, "11111");
    strcpy(msg.serverId, "00001");
    strcpy(msg.r2, "lwdyyds");
    CodeFactory * factory = new Respond_Factory(&msg);
    Code* code = factory->createcode();
    char* outData;
    int len = 0;
    //进行编码
    code->encode(&outData, len);
    delete factory;
    delete code;

    factory = new Respond_Factory();
    code = factory->createcode();

    RespondMsg* data = (RespondMsg*)code->decode(outData, len);

    cout << "rv = " << data->rv << '\n';
    cout << "clientId = " << data->clientId << '\n';
    cout << "serverId = " << data->serverId << '\n';
    cout << "r2 = " << data->r2 << '\n';
    cout << "seckeyid = " << data->seckeyid << '\n';

    delete factory;
    delete code;
    return 0;
}
*/

//测试随机生成字符
/*
int main()
{
    srand((unsigned)time(NULL));
    char data[64] = {0};
    Client client;
    client.get(data, sizeof(data));
    printf("data = %s\n", data);
    return 0;
}
*/

//测试使用共享内存
/*
int main()
{
    NodeSHMInfo info;
    info.status = 0;
    info.seckeyID = 1;
    strcpy(info.clientID, "00001");
    strcpy(info.serverID, "22221");
    strcpy(info.seckey, "33333");

    //Sec_Memtory sec(123456, 10);
    Sec_Memtory sec(123456);
    //sec.Write_Memory(&info);
    //sec.print_Memory();
    memset(&info, 0x00, sizeof(NodeSHMInfo));
    sec.Read_Memory("11111", "22221", &info);
    cout << "status = " << info.status << '\n';
    cout << "clientID = " << info.clientID << '\n';
    cout << "serverID = " << info.serverID << '\n';
    cout << "seckey = " << info.seckey << '\n';
    cout << "seckeyID = " << info.seckeyID << '\n';


    return 0;
}
*/


/*
int main()
{
    char key[64];
    char data[64] = "sdfdsfsd43khr3jehkdshkfkjwdshfjsssfkssfsdfasdfdfsdhfewejkewhjks";
    unsigned char md[SHA256_DIGEST_LENGTH];
    memset(key, 0x00, sizeof(key));
    memset(md, 0x00, sizeof(md));
    unsigned int len = 0;
    sprintf(key, "@%s%s@", "111111", "222222");
    HMAC(EVP_sha256(), key, strlen(key), (unsigned char*)data, strlen(data), md, &len);

    char authCode[65];
    for(int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        sprintf(&authCode[2 * i], "%02x", md[i]);
    }
    cout << "key:" << key << endl;
	cout << "r1:" << data << endl;
	cout << "authCode:" << authCode << endl;
    return 0;
    //e2fd719ed8d5aef4391ea36bc6d77d855f3c0892f825b009748d6973ed09d901
    //sdfdsfsd43khr3jehkdshkfkjwdshfjsssfkssfsdfasdfdfsdhfewejkewhjks
}
*/

/*
#include<iostream>
#include"Sec_Key_Memtory.h"
#include<string.h>
using namespace std;

int main()
{
    NodeSHMInfo info;
    //Sec_Memtory sec(123456, 10);
    Sec_Key_Memtory sec(0x123456);
    //sec.Write_Memory(&info);
    sec.print_Memory();
    memset(&info, 0x00, sizeof(NodeSHMInfo));
    sec.Read_Memory("1", "1111", &info);
    cout << "status = " << info.status << '\n';
    cout << "clientID = " << info.clientID << '\n';
    cout << "serverID = " << info.serverID << '\n';
    cout << "seckey = " << info.seckey << '\n';
    cout << "seckeyID = " << info.seckeyID << '\n';
    

    return 0;
}
*/

