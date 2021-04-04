#ifndef __MY_MSG_CLIENT__
#define __MY_MSG_CLIENT__

#include "TaskMgrApp.h"
#include "MyProtoMsg.h"
#include "MyMsgQueue.h"

class MyMsgServer: public MyMsgQueue
{
private:
    MyMsgServer();

public:
    ~MyMsgServer();
    static MyMsgServer* Instance();

    virtual void DispatchMessage(MyProtoMsg* pMsg);
    void SendCmdMsgToTask(MyProtoMsg* pMsg);
    int SendCmdMsgToServer(MyProtoMsg* pMsg);

private:
    static ACE_Thread_Mutex m_mutex;
    static MyMsgServer *m_instance;
};

#endif
