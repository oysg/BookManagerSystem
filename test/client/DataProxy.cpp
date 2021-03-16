#include "DataProxy.h"
#include "commonfunc.h"

extern CommonFunc g_commonFunc;
#define MSG_ADDLEVEL 1000


void DataProxy::AddLevel(int nLevel)
{
    int level = dataModel.GetLevel();
    dataModel.SetLevel(level + nLevel);

    level = dataModel.GetLevel();
    CommonFunc::Instance()->SendNotifyCationToView(MSG_ADDLEVEL, &level);
}