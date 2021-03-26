#include "iMapCmdMsg.h"

iMapCmdMsg::iMapCmdMsg()
{
    m_nMagicNum = 0;
    m_nVersion = 0;
    m_nMsgID = 0;
    m_nMrbCmdMsg = 0;
    m_nMsgType = 0;
    m_nMsgHeaderLength = 0;
    m_nMsgBodyLength = 0;
    m_strBody = "";
    string str = serializeHeader();
    m_nMsgHeaderLength = str.size();
}

iMapCmdMsg::~iMapCmdMsg()
{

}

void iMapCmdMsg::SetMagicNum(int nMagicNum)
{
    m_nMagicNum = nMagicNum;
}

int iMapCmdMsg::GetMagicNum()
{
    return m_nMagicNum;
}

void iMapCmdMsg::SetVersion(int nVersion)
{
    m_nVersion = nVersion;
}

int iMapCmdMsg::GetVersion()
{
    return m_nVersion;
}

void iMapCmdMsg::SetMsgID(int nMsgID)
{
    m_nMsgID = nMsgID;
}

int iMapCmdMsg::GetMsgID()
{
    return m_nMsgID;
}

void iMapCmdMsg::SetMrbCmdMsg(int nMrbCmdMsg)
{
    m_nMrbCmdMsg = nMrbCmdMsg;
}

int iMapCmdMsg::GetMrbCmdMsg()
{
    return m_nMrbCmdMsg;
}

void iMapCmdMsg::SetMsgType(int nMsgType)
{
    m_nMsgType = nMsgType;
}

int iMapCmdMsg::GetMsgType()
{
    return m_nMsgType;
}

void iMapCmdMsg::SetMsgBodyLength(int nLength)
{
    m_nMsgBodyLength = nLength;
}

int iMapCmdMsg::GetMsgBodyLength()
{
    return m_nMsgBodyLength;
}

int iMapCmdMsg::GetMsgHeaderLength()
{
    return m_nMsgHeaderLength;
}

void iMapCmdMsg::SetBody(string strBody)
{
    m_strBody = strBody;
}

string iMapCmdMsg::GetBody()
{
    return m_strBody;
}