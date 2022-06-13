#pragma once
#include"CodeFactory.h"
#include"Respond_Code.h"

class Respond_Factory : public CodeFactory
{
public:
    Respond_Factory();
    Respond_Factory(RespondMsg* msg);
    ~Respond_Factory();

    Code* createcode(); 

private:
    RespondMsg m_msg;
    bool m_flag;
};


