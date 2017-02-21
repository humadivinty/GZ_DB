#ifndef THREAD_DB_LOCAL
#define THREAD_DB_LOCAL
#include <QThread>
#include "customresultlist.h"
#include "dbcontrl.h"

class Thread_DB_Local : public QThread
{
    Q_OBJECT
public:
    Thread_DB_Local(QString ThreadName, CustomResultList* ResultList, QObject* parent = 0);
    ~Thread_DB_Local();

    void run();
    void StopThread();
private:
    void InitParameter();
    bool WriteLog(const QString qstrLog);

private:
    QString m_qstrThreadName;
    QString m_qstrCachePath;
    CustomResultList* m_pResultList;
    DBINFO m_dbInfo;
    bool m_bExit;

signals:
    void Signal_DBConnectingChange(bool changeValue);
};

#endif // THREAD_DB_LOCAL

