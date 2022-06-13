#pragma once
#include<json/json.h>
#include"Sec_Key_Server.h"

class Jsoncpp
{
private:
    /* data */
public:
    Jsoncpp();
    ~Jsoncpp();
    bool ReadJosn(ServerInfo *info);
};