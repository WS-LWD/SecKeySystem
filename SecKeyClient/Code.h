#pragma once
#include"BaseASN1Code.h"

class Code : public BaseASN1Code
{
public:
    Code();
    ~Code();

    virtual void encode(char** outData, int &len);
    virtual void* decode(char* inData, int len);

};
