#include<iostream>
#include"Jsoncpp.h"
using namespace std;
void usage()
{
	printf("\n  /*************************************************************/");
	printf("\n  /*************************************************************/");
	printf("\n  /*     0.密钥协商                                            */");
	printf("\n  /*     1.密钥校验                                          */");
	printf("\n  /*     2.密钥注销                                            */");
	printf("\n  /*     3.密钥查看                                           */");
	printf("\n  /*     其他.退出系统                                        */");
	printf("\n  /*************************************************************/");
	printf("\n  /*************************************************************/");
	printf("\n\n  选择:");

}

int main()
{
    ClientInfo info;
    Jsoncpp json;
    json.ReadJosn(&info);
    Client client(&info);
    int index;
    bool falg = true;
    while (falg)
    {
        usage();
        cin >> index;
        switch (index)
        {
        case RequestCode::CONSULT:          //秘钥协商
            client.SecKey_Consult();
            break;
        case RequestCode::CHECK:            //秘钥校验
            client.SecKey_Check();
            break;
        case RequestCode::CANCELL:          //秘钥注销
            client.SecKey_Cancel();
            break;
        case RequestCode::BEFORE_VIEW:      //秘钥查看 --历史版本
            //Sec_key_View();
            break;
        default:
            falg = false;
            break;
        }
    }

    NodeSHMInfo info1;
    Sec_Key_Memtory sec(info.shmKey);
    //sec.Write_Memory(&info);
    //sec.print_Memory();
    memset(&info1, 0x00, sizeof(NodeSHMInfo));
    sec.Read_Memory("1", "1111", &info1);
    cout << "status = " << info1.status << '\n';
    cout << "clientID = " << info1.clientID << '\n';
    cout << "serverID = " << info1.serverID << '\n';
    cout << "seckey = " << info1.seckey << '\n';
    cout << "seckeyID = " << info1.seckeyID << '\n';
    
    
    return 0;   
}

