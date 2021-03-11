#ifndef __LEND_LIST_DAT_IMPL__
#define __LEND_LIST_DAT_IMPL__

#include "LendListDao.h"
#include "TableLendList.h"
#include "SQLConnection.h"
#include "common.h"

class LendListDaoImpl: public LendListDao
{

public:
    LendListDaoImpl();
    ~LendListDaoImpl(){};
    
    virtual int AddLendData(TableLendList &bookInfo) throw (SQLException);
    virtual int DeleteLendDataByField(const FieldCond &fieldCond) throw (SQLException);
    virtual int DeleteAllLendData() throw (SQLException);
    virtual int UpdateLendDataByField(const vector<FieldCond> &setFieldCond, const FieldCond &fieldCond) throw (SQLException);
    virtual int QueryLendDataByField(const FieldCond& fieldCond, list<vector<string> > &listBookInfo) throw (SQLException);

private:
    MYSQL *m_pMySQLConn;
};

#endif //__LEND_LIST_DAT_IMPL__