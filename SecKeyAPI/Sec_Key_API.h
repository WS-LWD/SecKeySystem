#include"Sec_Key_Memtory.h"
#include"AES.h"
#include"Base64.h"
#include<string>


int const CLIENT = 0;
int const SERVER = 1;
class SecKey
{

public:
    SecKey(const char* path); //配置文件的路径

    ~SecKey();
    string endataAES(const string& strSrc);
    string dedataAES(const string& strSrc);

private:
    string EncryptionAES(const string& strSrc, const char* seckey);
    string DecryptionAES(const string& strSrc, const char* seckey);

private:
    int m_seckeyid;
    Sec_Key_Memtory *m_shm;
};  


