#ifndef THREAD_MOVERESULTTODBLIST
#define THREAD_MOVERESULTTODBLIST
#include <QThread>

class Thread_MoveResultToDBList : public QThread
{
    Q_OBJECT
public:
    Thread_MoveResultToDBList(void* pCamera, QObject* parent = 0);
    ~Thread_MoveResultToDBList();

    void run();
    void StopThread();
private:
    void* m_pCamera;
    bool m_bExit;
    bool m_bSaveDB_Local;
    bool m_bSaveDB_Remote;

    QString m_qstrLocalCachePath_MoveThread;
    QString m_qstrRemoteCachePath_MoveThread;
private:
    void InitParameter();
};

#endif // thread_MoveResultToDBList

