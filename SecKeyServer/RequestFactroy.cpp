#include"RequestFactroy.h"


RequestFactroy::RequestFactroy() : m_Iscode(false)
{

}

RequestFactroy::RequestFactroy(RequestMsg* msg) : m_Iscode(true), m_request(msg)
{

}

RequestFactroy::~RequestFactroy()
{

}

Code* RequestFactroy::createcode()
{
    if(m_Iscode)
    {
        return new RequestCode(m_request);
    }
    else
    {
        return new RequestCode();
    }
    return nullptr;
}

