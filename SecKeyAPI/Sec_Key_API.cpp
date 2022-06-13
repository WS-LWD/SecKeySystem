#include"Sec_Key_API.h"
#include<openssl/aes.h>
#include<iostream>
#include<json/json.h>
#include <fstream>
using namespace std;
using namespace Json;

SecKey::SecKey(const char* path)
{
    ifstream ifs(path);
	Reader r;
	Value root;
	r.parse(ifs, root);
    m_shm = new Sec_Key_Memtory(root["shmkey"].asCString(), root["maxnode"].asInt());
    m_seckeyid = m_shm->Read_Frist_Memory();
}

SecKey::~SecKey()
{
    delete m_shm;
}

string SecKey::endataAES(const string& strSrc)
{
    //获取
    NodeSHMInfo shminfo;
    m_shm->Read_Memory(m_seckeyid, &shminfo);
    string str = EncryptionAES(strSrc, shminfo.seckey);
    string ret = to_string(m_seckeyid) + "@" + str;
    return ret;
}
string SecKey::dedataAES(const string& strSrc)
{
    NodeSHMInfo shminfo;
    string ret;
    string Substr;
    for(int i = 0; i < strSrc.size(); ++i)
    {
        if(strSrc[i] != '@')
        {
            ret = ret + strSrc[i];
        }
        else
        {
            Substr = strSrc.substr(i + 1, strSrc.size() - i - 1);
            break;
        }
    }
    m_seckeyid = atoi(ret.data());
    m_shm->Read_Memory(m_seckeyid, &shminfo);
    return DecryptionAES(Substr, shminfo.seckey);
}


string SecKey::EncryptionAES(const string& strSrc, const char* seckey)
{
    char g_iv[16];
	memset(g_iv, 'a', sizeof(g_iv));
	size_t length = strSrc.length();
	int block_num = length / BLOCK_SIZE + 1;
	//明文
	char* szDataIn = new char[block_num * BLOCK_SIZE + 1];
	memset(szDataIn, 0x00, block_num * BLOCK_SIZE + 1);
	strcpy(szDataIn, strSrc.c_str());
 
	//进行PKCS7Padding填充。
	int k = length % BLOCK_SIZE;
	int j = length / BLOCK_SIZE;
	int padding = BLOCK_SIZE - k;
	for (int i = 0; i < padding; i++)
	{
		szDataIn[j * BLOCK_SIZE + k + i] = padding;
	}
	szDataIn[block_num * BLOCK_SIZE] = '\0';
 
	//加密后的密文
	char *szDataOut = new char[block_num * BLOCK_SIZE + 1];
	memset(szDataOut, 0, block_num * BLOCK_SIZE + 1);
 
	//进行进行AES的CBC模式加密
	AES aes;
	aes.MakeKey(seckey, g_iv, 16, 16);
	aes.Encrypt(szDataIn, szDataOut, block_num * BLOCK_SIZE, AES::CBC);
	string str = base64_encode((unsigned char*) szDataOut, block_num * BLOCK_SIZE);
	delete[] szDataIn;
	delete[] szDataOut;
	return str;
}
string SecKey::DecryptionAES(const string& strSrc, const char* seckey)
{
    char g_iv[16];
	memset(g_iv, 'a', sizeof(g_iv));
	string strData = base64_decode(strSrc);
	size_t length = strData.length();
	//密文
	char *szDataIn = new char[length + 1];
	memcpy(szDataIn, strData.c_str(), length+1);
	//明文
	char *szDataOut = new char[length + 1];
	memcpy(szDataOut, strData.c_str(), length+1);
 
	//进行AES的CBC模式解密
	AES aes;
	aes.MakeKey(seckey, g_iv, 16, 16);
	aes.Decrypt(szDataIn, szDataOut, length, AES::CBC);
 
	//去PKCS7Padding填充
	if (0x00 < szDataOut[length - 1] <= 0x16)
	{
		int tmp = szDataOut[length - 1];
		for (int i = length - 1; i >= length - tmp; i--)
		{
			if (szDataOut[i] != tmp)
			{
				memset(szDataOut, 0, length);
				cout << "去填充失败！解密出错！！" << endl;
				break;
			}
			else
				szDataOut[i] = 0;
		}
	}
	string strDest(szDataOut);
	delete[] szDataIn;
	delete[] szDataOut;
	return strDest;
}