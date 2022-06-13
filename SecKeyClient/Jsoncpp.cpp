#include"Jsoncpp.h"
#include<iostream>
#include <fstream>
#include <string.h>
using namespace std;
//包含jsoncpp的命名空间
using namespace Json;

Jsoncpp::Jsoncpp()
{
}

Jsoncpp::~Jsoncpp()
{
}

bool Jsoncpp::ReadJosn(ClientInfo *info)
{
    
    //打开磁盘文件
	ifstream ifs("Client.json");
	Reader r;
	Value root;
	r.parse(ifs, root);
	//通过root对象取数据
       
	if(root.isObject())
	{
        
        strcpy(info->shmKey, root["shmkey"].asCString());
        strcpy(info->clinetID, root["clientid"].asCString());
        strcpy(info->serverIP, root["serverip"].asCString());
        strcpy(info->serverID, root["serverid"].asCString());
        info->serverPort = root["server_prot"].asInt();
        info->maxNode = root["maxnode"].asInt();
        
	}
    
    ifs.close();
    return true;
}