#include"Respond_Factory.h"



Respond_Factory::Respond_Factory() : m_flag(false)
{

}

Respond_Factory::Respond_Factory(RespondMsg* msg) : m_flag(true)
{
    memset(&m_msg, 0x00, sizeof(RespondMsg));
    memcpy(&m_msg, msg, sizeof(RespondMsg));
}

Respond_Factory::~Respond_Factory()
{

}

Code* Respond_Factory::createcode()
{
    if(m_flag)
    {
        return new Respond_Code(&m_msg);
    }
    else
    {
        return new Respond_Code();
    }
    return nullptr;
}