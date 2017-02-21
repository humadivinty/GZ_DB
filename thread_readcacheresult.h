#ifndef THREAD_READCACHERESULT
#define THREAD_READCACHERESULT
#include<QThread>
#include "customresultlist.h"

class Thread_ReadCacheResult :public QThread
{
    Q_OBJECT
public:
    Thread_ReadCacheResult(QString threadName, QObject* parent = 0 );
    ~Thread_ReadCacheResult();

    void SetResutlListAndType(CustomResultList* pResultList, int ReultType);
    void StopThread();
private:

    void run();
private:
    void InitParameter();
    bool WriteLog(const QString qstrLogBuf);

private:
    QString m_qstrThreadName;
    QString m_qstrCachePath;
    CustomResultList* m_pReultList;
    int m_iResultType;        //1为本地结果， 2为远程结果
    bool m_bExit;
    bool m_bIsUpLoad;

public slots:
    void slot_ChangeUpLoadFlag(bool FlagStatus);
};

#endif // THREAD_READCACHERESULT

