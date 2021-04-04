#include "MyMsgQueue.h"

#define REQUEST_MSG_TYPE 200
#define RESPONSE_MSG_TYPE 201
#define END_MSG_TYPE 300

ACE_Thread_Mutex MyMsgQueue::m_mutex;
MyMsgQueue* MyMsgQueue::m_instance = NULL;

MyMsgQueue::MyMsgQueue()
:m_mMsgMutex(), m_mMsgCond(m_mMsgMutex)
{

}

MyMsgQueue::~MyMsgQueue()
{
}

uint8_t* MyMsgQueue::encode(MyProtoMsg* pMsg, uint32_t& length)
{
    uint8_t* pData = NULL; 
    Json::FastWriter fwriter; 

                            
    string bodyStr = fwriter.write(pMsg->Body);


    length = MY_PROTO_HEAD_SIZE + (uint32_t)bodyStr.size();
    pMsg->Header.nMsgLength = length; 
                         
    pData = new uint8_t[length];

    headEncode(pData, pMsg);
    memcpy(pData + MY_PROTO_HEAD_SIZE, bodyStr.data(), bodyStr.size());

    return pData; 
}


void MyMsgQueue::headEncode(uint8_t* pData, MyProtoMsg* pMsg)
{
 
    *pData = MY_PROTO_MAGIC;
    ++pData; 


    *pData = 1;
    ++pData; 

    *(uint16_t*)pData = pMsg->Header.nSendProc;
    pData += 2; 

    *(uint16_t*)pData = pMsg->Header.nRecvProc;
    pData += 2;
    
    *(uint32_t*)pData = pMsg->Header.nTaskMgrID;
    pData += 4; //��ǰ�ƶ������ֽڣ���������ϢnCmdMsg
    
    *(uint32_t*)pData = pMsg->Header.nTaskID;
    pData += 4; //��ǰ�ƶ������ֽڣ���������ϢnCmdMsg
    
    *(uint32_t*)pData = pMsg->Header.nMsgID;
    pData += 4; //��ǰ�ƶ������ֽڣ���������ϢnCmdMsg

    *(uint32_t*)pData = pMsg->Header.nCmdMsg;
    pData += 4; //��ǰ�ƶ������ֽڣ���������ϢnMsgType

    *(uint32_t*)pData = pMsg->Header.nMsgType;
    pData += 4; //��ǰ�ƶ������ֽڣ���������ϢnMsgRet

    *(uint32_t*)pData = pMsg->Header.nMsgRet;
    pData += 4; //��ǰ�ƶ������ֽڣ���������ϢnMsgLength

    *(uint32_t*)pData = pMsg->Header.nMsgLength;
}

void MyMsgQueue::init()
{
    m_mCurParserStatus = ON_PARSER_INIT;
}

void MyMsgQueue::clear()
{
    MyProtoMsg* pMsg = NULL;
    while (!m_mMsgQ.empty())
    {
        pMsg = m_mMsgQ.front();
        delete pMsg;
        m_mMsgQ.pop();
    }
}

bool MyMsgQueue::empty()
{
    return m_mMsgQ.empty();
}

void MyMsgQueue::pop()
{
    m_mMsgQ.pop();
}

void MyMsgQueue::push(MyProtoMsg *pMsg)
{
    m_mMsgQ.push(pMsg);
    m_mMsgCond.signal();
}


MyProtoMsg* MyMsgQueue::front()
{
    return m_mMsgQ.front();
}

bool MyMsgQueue::GetMessage(MyProtoMsg* pMsg)
{
    while (true)
    {
        m_mMsgMutex.acquire();
        if(!empty())
        {
            pMsg = front();
            if(pMsg->Header.nMsgType == END_MSG_TYPE)
            {
                pop();
                m_mMsgMutex.release();
                break;
            }

            pop();
            m_mMsgMutex.release();
            return true;
        }
        else
        {
            m_mMsgCond.wait();
        }
        
    }
    
    return false;
}

void MyMsgQueue::DispatchMessage(MyProtoMsg* pMsg)
{

}


bool MyMsgQueue::parser(void* data, size_t len)
{
    if (len <= 0)
        return false;

    uint32_t curLen = 0; 
    uint32_t parserLen = 0; 
    uint8_t* curData = NULL; 

    curData = (uint8_t*)data;

    while (len--)
    {
        m_mCurReserved.push_back(*curData);
        ++curData;
    }

    curLen = m_mCurReserved.size();
    curData = (uint8_t*)&m_mCurReserved[0];

    while (curLen > 0)
    {
        bool parserBreak = false;

        if (ON_PARSER_INIT == m_mCurParserStatus || ON_PARSER_BODY == m_mCurParserStatus) 
        {
            if (!parserHead(&curData, curLen, parserLen, parserBreak))
            {
                return false;
            }
            if (parserBreak)
                break; 
        }

        if (ON_PARSER_HEAD == m_mCurParserStatus)
        {
            if (!parserBody(&curData, curLen, parserLen, parserBreak))
            {
                return false;
            }
            if (parserBreak)
                break;
        }

        if (ON_PARSER_BODY == m_mCurParserStatus)
        {
            MyProtoMsg* pMsg = NULL;
            pMsg = new MyProtoMsg;
            *pMsg = m_mCurMsg;
            m_mMsgMutex.acquire();
            m_mMsgQ.push(pMsg);
            m_mMsgCond.signal();
            m_mMsgMutex.release();
            ACE_DEBUG((LM_DEBUG, "(%P|%t|)MyProtoDecode::parser>>m_mMsgQ.size:%d\n", m_mMsgQ.size()));
        }

        if (parserLen > 0)
        {
            //ɾ���Ѿ��������������ֽ���
            m_mCurReserved.erase(m_mCurReserved.begin(), m_mCurReserved.begin() + parserLen);
        }
    }

    return true;
}

bool MyMsgQueue::parserHead(uint8_t** curData, uint32_t& curLen, uint32_t& parserLen, bool& parserBreak)
{
    if (curLen < MY_PROTO_HEAD_SIZE)
    {
        parserBreak = true;
        return true;
    }

    uint8_t* pData = *curData;

    m_mCurMsg.Header.nMagic = *pData;
    pData++;

    //�ж�У����Ϣ
    if (MY_PROTO_MAGIC != m_mCurMsg.Header.nMagic)
        return false; //���ݳ���

    //����������У��İ汾��
    m_mCurMsg.Header.nVersion = *pData;
    pData++;

    //�������ͽ��̺�
    m_mCurMsg.Header.nSendProc = *(uint16_t*)pData;
    pData += 2;

    //�������ս��̺�
    m_mCurMsg.Header.nRecvProc = *(uint16_t*)pData;
    pData += 2;

    m_mCurMsg.Header.nTaskMgrID = *(uint32_t*)pData;
    pData += 4;

    m_mCurMsg.Header.nTaskID = *(uint32_t*)pData;
    pData += 4;
    
    //����������ϢID
    m_mCurMsg.Header.nMsgID = *(uint32_t*)pData;
    pData += 4;

    //��������������
    m_mCurMsg.Header.nCmdMsg = *(uint32_t*)pData;
    pData += 4;

    //����������Ϣ����
    m_mCurMsg.Header.nMsgType = *(uint32_t*)pData;
    pData += 4;

    //����������Ϣ����ֵ
    m_mCurMsg.Header.nMsgRet = *(uint32_t*)pData;
    pData += 4;

    //����������Ϣ�峤��
    m_mCurMsg.Header.nMsgLength = *(uint32_t*)pData;

    //�ж����ݳ����Ƿ񳬹�ָ���Ĵ�С
    if (m_mCurMsg.Header.nMsgLength > MY_PROTO_MAX_SIZE)
        return false;

    //������ָ����ǰ�ƶ�����Ϣ��λ��,������Ϣͷ��С
    (*curData) += MY_PROTO_HEAD_SIZE;
    curLen -= MY_PROTO_HEAD_SIZE;
    parserLen += MY_PROTO_HEAD_SIZE;
    m_mCurParserStatus = ON_PARSER_HEAD;

    return true;
}

bool MyMsgQueue::parserBody(uint8_t** curData, uint32_t& curLen, uint32_t& parserLen, bool& parserBreak)
{
    uint32_t JsonSize = m_mCurMsg.Header.nMsgLength - MY_PROTO_HEAD_SIZE; //��Ϣ��Ĵ�С
    if (curLen < JsonSize)
    {
        parserBreak = true; //���ݻ�û����ȫ������ǻ�Ҫ�ȴ�һ�����ݵ��ˣ��ٽ�����Ϣ�塣���ڱ�־û�䣬һ�ỹ�ǽ�����Ϣ��
        return true;
    }

    Json::Reader reader; //Json������
    if (!reader.parse((char*)(*curData),
        (char*)((*curData) + JsonSize), m_mCurMsg.Body, false)) //false��ʾ����ע��
        return false; //�������ݵ�body��

                      //����ָ����ǰ�ƶ�
    (*curData) += JsonSize;
    curLen -= JsonSize;
    parserLen += JsonSize;
    m_mCurParserStatus = ON_PARSER_BODY;

    return true;
}