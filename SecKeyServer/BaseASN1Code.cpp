#include<iostream>
#include<string>
#include"BaseASN1Code.h"

using namespace std;

BaseASN1Code::BaseASN1Code() : BaseASN1()
{

}

BaseASN1Code::~BaseASN1Code()
{
	
}

int BaseASN1Code::Write_Header_node(int val)
{
    int ret = DER_ItAsn1_WriteInteger(val, &m_header);
    if (ret != 0)
	{
		return ret;
	}
    m_next = m_header;
    return 0;
}

int BaseASN1Code::Write_Header_node(char* str, int len)
{
    DER_ITCAST_String_To_AnyBuf(&m_temp, (unsigned char*)str, len);
	int ret = DER_ItAsn1_WritePrintableString(m_temp, &m_header);
	if (ret != 0)
	{
		return ret;
	}
	m_next = m_header;
	DER_ITCAST_FreeQueue(m_temp);

	return 0;
}

int BaseASN1Code::Write_Next_node(int val)
{
    int ret = DER_ItAsn1_WriteInteger(val, &m_next->next);
    if (ret != 0)
	{
		return ret;
	}
    m_next = m_next->next;
    return 0;
}

int BaseASN1Code::Write_Next_node(char* str, int len)
{
	int ret = EncodeChar(str, len, &m_next->next);
	if (ret != 0)
	{
		return ret;
	}
	m_next = m_next->next;

	return 0;
}

int BaseASN1Code::Read_Header_node(int &val)
{
	int ret = DER_ItAsn1_ReadInteger(m_header, (ITCAST_UINT32*)&val);
	if (ret != 0)
	{
		return ret;
	}
	m_next = m_header->next;
	return 0;
}

int BaseASN1Code::Read_Header_node(char* str)
{
	int ret = DER_ItAsn1_ReadPrintableString(m_header, &m_temp);
	if (ret != 0)
	{
		return ret;	
	}	
	memcpy(str, m_temp->pData, m_temp->dataLen);
	DER_ITCAST_FreeQueue(m_temp);

	m_next = m_header->next;
	return 0;
}

int BaseASN1Code::Read_Next_node(int &val)
{
	
	int ret = DER_ItAsn1_ReadInteger(m_next, (ITCAST_UINT32*)&val);
	if (ret != 0)
	{
		return ret;
	}
	m_next = m_next->next;
	return 0;
}

int BaseASN1Code::Read_Next_node(char* str)
{
	DER_ItAsn1_ReadPrintableString(m_next, &m_temp);
	memcpy(str, m_temp->pData, m_temp->dataLen);
	DER_ITCAST_FreeQueue(m_temp);

	m_next = m_next->next;
	return 0;
}

int BaseASN1Code::Pack_queue(char** outData, int &len)
{
	DER_ItAsn1_WriteSequence(m_header, &m_temp);
	// 传出参数赋值
	*outData = (char*)m_temp->pData;
	len = m_temp->dataLen;
	// 释放链表
	DER_ITCAST_FreeQueue(m_header);

	return 0;
}

int BaseASN1Code::unPack_queue(char* inData, int &len)
{
	DER_ITCAST_String_To_AnyBuf(&m_temp, (unsigned char*)inData, len);
	DER_ItAsn1_ReadSequence(m_temp, &m_header);
	DER_ITCAST_FreeQueue(m_temp);

	return 0;
}

