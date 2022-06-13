#pragma once
#include<string>
#include"Sec_Key_Memtory.h"
#include<mysql/mysql.h>

using namespace std;

class Mysql_database
{

public:
    Mysql_database();
    ~Mysql_database();
    bool connectDB(const char* connstr, const char* user, const char* passwd, const char* mydb, unsigned int port);
	// 得到keyID
	int getKeyID();
	bool updataKeyID(int keyID);
	bool writeSecKey(NodeSHMInfo *pNode);
	void closeDB();

	bool check_client(string clientID);

	bool updata_status(char* clientID, char* serverID);
private:
	// 获取当前时间, 并格式化为字符串
	string getCurTime();

private:
    MYSQL *m_mysql;
};


