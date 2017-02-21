#ifndef THREAD_CONNECT_CAMERA
#define THREAD_CONNECT_CAMERA
#include"dbcontrl.h"

class Thread_Connect_Camera : public QThread
{
    Q_OBJECT
public:
    Thread_Connect_Camera(QObject* parent = 0);
    ~Thread_Connect_Camera();

    void run();

    void SetCameraAndInfo(Camera6467** pCameraGroup, CameraInfo** pCameraInfoGroup);
    void SetLocalAndRemoteList(CustomResultList* pLocalList, CustomResultList* pRemoteList);
private:
    void InitParameter();
    bool WriteLog(const QString qstrLogBuff);
    Camera6467** m_pCameraGroup;
    CameraInfo** m_pCameraInfoGroup;
    DBINFO m_dbInfo;
    CustomResultList* g_pLocalList;
    CustomResultList* g_pRemoteList;
};

#endif // THREAD_CONNECT_CAMERA

