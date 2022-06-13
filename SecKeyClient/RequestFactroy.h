#pragma once
#include"CodeFactory.h"
#include"RequestCode.h"

class RequestFactroy : public CodeFactory
{
public:
    
    RequestFactroy();
    RequestFactroy(RequestMsg* m_request);
    ~RequestFactroy();

    virtual Code* createcode(); 

private:
    RequestMsg* m_request;
    bool m_Iscode;
};

