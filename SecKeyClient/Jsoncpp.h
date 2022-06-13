#pragma once
#include<json/json.h>
#include"Client.h"

class Jsoncpp
{
private:
    /* data */
public:
    Jsoncpp();
    ~Jsoncpp();
    bool ReadJosn(ClientInfo *info);
};