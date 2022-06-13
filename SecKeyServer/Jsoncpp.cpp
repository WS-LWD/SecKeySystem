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

bool Jsoncpp::ReadJosn(ServerInfo *info)
{
    
    //打开磁盘文件
	ifstream ifs("Server.json");
	Reader r;
	Value root;
	r.parse(ifs, root);
	//通过root对象取数据
       
	if(root.isObject())
	{
        
        strcpy(info->shmkry, root["shmkey"].asCString());
        strcpy(info->serverIP, root["serverip"].asCString());
        strcpy(info->serverID, root["serverid"].asCString());
        info->serverPORT = root["server_prot"].asInt();
        info->MaxNode = root["maxnode"].asInt();
        strcpy(info->mysql_Use, root["mysql_use"].asCString());
        strcpy(info->mysql_db, root["mysql_db"].asCString());
        strcpy(info->mysql_Passwd, root["mysql_passwd"].asCString());
        info->mysql_prot = root["mysql_port"].asInt();
        
	}
    
    ifs.close();
    return true;
}