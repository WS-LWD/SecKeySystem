#include"CodeFactory.h"
#include<iostream>
using namespace std;
CodeFactory::CodeFactory()
{

}

CodeFactory::~CodeFactory()
{

}

Code* CodeFactory::createcode()
{
    cout << __FILE__ << __LINE__ << '\n';
    return nullptr;
}