#ifndef THREAD_DB_SAVESTATUS
#define THREAD_DB_SAVESTATUS
#include "dbcontrl.h"

class Thread_DB_SaveStatus : public QThread
{
    Q_OBJECT
public:
    Thread_DB_SaveStatus(QString qstrThreadName,Camera6467** pCameraList, QObject* parent = 0 );
    ~Thread_DB_SaveStatus();

    void run();
    void StopThread();
private:
    void InitParameter();
    bool WriteLog(const QString qstrLogBuf);

private:
    QString m_qstrThreadName;
    Camera6467** m_pCameraList;
    DBINFO m_dbInfo_Local;
    DBINFO m_dbInfo_Remote;
    bool m_bExit;
    bool m_bUpLocalDB;
    bool m_bUpRemoteDB;
    bool m_bSynTimeEnable;
};


#endif // THREAD_DB_SAVESTATUS

