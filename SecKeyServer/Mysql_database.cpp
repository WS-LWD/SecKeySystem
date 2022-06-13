#include"Mysql_database.h"
#include <iostream>
#include <string.h>
#include <time.h>
using namespace std;

Mysql_database::Mysql_database(/* args */)
{
    
}

Mysql_database::~Mysql_database()
{
}

bool Mysql_database::connectDB(const char* connstr, const char* user, const char* passwd, const char* mydb, unsigned int port)
{
	// 1. 初始化环境
    m_mysql = mysql_init(NULL);
	// 2. 创建连接
     MYSQL *m_conn = mysql_real_connect(m_mysql, connstr, user, passwd, mydb, port, NULL, 0);
	if (m_conn == NULL)
	{
		return false;
	}
	mysql_set_character_set(m_mysql,"utf8");
	//设置自动提交	
	mysql_query(m_mysql, "set autocommit=1");
	cout << "数据库连接成功..." << endl;
	return true;
}

int Mysql_database::getKeyID()
{
	// 查询数据库
	const char* sql = "select ikeysn from keysn";
	// 执行数据查询
	// 该表只有一条记录
	if(mysql_query(m_mysql, sql) != 0)
	{
		
		printf("%s\n", mysql_error(m_mysql));
		return -1;
	}
	MYSQL_RES* res = mysql_store_result(m_mysql);
	if(res == nullptr)
        {
            printf("%s\n", mysql_error(m_mysql));
            return -1;
        }
	MYSQL_ROW rows = mysql_fetch_row(res);

	if (rows[0] == nullptr) 
	{
		return -1;
	}
	int ret = atoi(rows[0]);
	cout << "KeyID" << ret << '\n';
	mysql_free_result(res);
	return ret;
}

// 秘钥ID在插入的时候回自动更新, 也可以手动更新
bool Mysql_database::updataKeyID(int keyID)
{
	// 更新数据库

	string sql = "update keysn set ikeysn = " + to_string(keyID);
	if(mysql_query(m_mysql, sql.c_str()) != 0)
	{
		return false;
	}
	return true;
}

// 将生成的秘钥写入数据库
// 更新秘钥编号
bool Mysql_database::writeSecKey(NodeSHMInfo *pNode)
{
	//insert into seckeyinfo values('%s','%s',%d,date_format('%s','%%Y%%m%%d%%H%%i%%s'),%d,'%s')", 
	//pnode->clientID, pnode->serverID, pnode->seckeyID, getCurrTime().data(), pnode->status, pnode->seckey)  
	//(clientid, serverid, keyid, createtime, state, seckey)

	// 组织待插入的sql语句
	//date_format('2022-06-10 00:31:16','%Y%m%d%H%i%s') 
	char sql[1024] = { 0 };
	memset(sql, 0x00, sizeof(sql));
	sprintf(sql, "insert into seckeyinfo values('%s', '%s', %d, date_format('%s','%%Y%%m%%d%%H%%i%%s') , %d, '%s') ", pNode->clientID, pNode->serverID, pNode->seckeyID, getCurTime().data(), 0, pNode->seckey);
	cout << "insert sql: " << sql << endl;
	
	if (mysql_query(m_mysql, sql) != 0)
	{
		
		printf("%s\n", mysql_error(m_mysql));
		return false;
	}
	return true;
}

void Mysql_database::closeDB()
{
	if(m_mysql != nullptr)
	{
		mysql_close(m_mysql);
	}
}

string Mysql_database::getCurTime()
{
	time_t t = time(NULL);
	char ch[64] = {0};
	char result[100] = {0};
	strftime(ch, sizeof(ch) - 1, "%Y-%m-%d %H:%M:%S", localtime(&t));
	sprintf(result, "%s", ch);
	return string(result);
}


bool Mysql_database::check_client(string clientID)
{
	string strsql = "select id from secnode where id = " + clientID;
	if (mysql_query(m_mysql, strsql.c_str()) != 0)
	{
		
		printf("%s\n", mysql_error(m_mysql));
		return false;
	}
	MYSQL_RES* res = mysql_store_result(m_mysql);
	if(res == nullptr)
    {
		printf("%s\n", mysql_error(m_mysql));
		return false;
    }
	MYSQL_ROW rows = mysql_fetch_row(res);

	if (rows[0] == nullptr) 
	{
		return false;
	}
	mysql_free_result(res);
	return true;
}

bool Mysql_database::updata_status(char* clientID, char* serverID)
{
	char strsql[1024];
	memset(strsql, 0x00, sizeof(strsql));
	sprintf(strsql , "update seckeyinfo set state = %d where clientid = '%s' and serverid = '%s' and state = %d", 1, clientID, serverID, 0); //1为不可用

	if (mysql_query(m_mysql, strsql) != 0)
	{
		printf("%s\n", mysql_error(m_mysql));
		return false;
	}
	return true;
}