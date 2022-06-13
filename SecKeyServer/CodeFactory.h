#pragma once
#include"Code.h"


//工厂类 --工厂模式
class CodeFactory
{
public:
    CodeFactory();
    ~CodeFactory();

    virtual Code* createcode(); 
};

