#ifndef THREAD_DB_REMOTE
#define THREAD_DB_REMOTE
#include <QThread>
#include "customresultlist.h"
#include "dbcontrl.h"

class Thread_DB_Remote : public QThread
{
    Q_OBJECT
 public:
    Thread_DB_Remote(QString ThreadName, CustomResultList* pResultList, QObject* parent = 0);
    ~Thread_DB_Remote();

    void run();
    void StopThread();
private:
    void InitParameter();
    bool WriteLog(const QString qstrLog);

private:
    QString m_qstrThreadName;
    CustomResultList* m_pResultList;
    QString m_qstrCachePath;
    DBINFO m_dbInfo;
    bool m_bExit;

signals:
    void Signal_DBConnectingChange(bool changeValue);
};

#endif // THREAD_DB_REMOTE

