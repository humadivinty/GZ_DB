#include "thread_connect_camera.h"


Thread_Connect_Camera::Thread_Connect_Camera(QObject *parent)
    :QThread(parent)
{
    qDebug()<<"0";
    m_pCameraGroup = NULL;
    m_pCameraInfoGroup = NULL;
    g_pLocalList = NULL;
    g_pRemoteList = NULL;
    InitParameter();
    qDebug()<<"1";
}

Thread_Connect_Camera::~Thread_Connect_Camera()
{
    m_pCameraGroup = NULL;
    m_pCameraInfoGroup = NULL;
    g_pLocalList = NULL;
    g_pRemoteList = NULL;
}

void Thread_Connect_Camera::run()
{
    m_dbInfo.qstrConnectName= "Connect_Thread";
    DBContrl dbLocal(m_dbInfo);
    dbLocal.ConnetToDB();
    if(NULL == m_pCameraInfoGroup || NULL == m_pCameraGroup)
        return;
    //qDebug()<<"step1";
    if(dbLocal.InitCameraInfo(m_pCameraInfoGroup))
    {
        //qDebug()<<"step2";
        for(int i = 0, j = 0; i< MAX_CAMERA_COUNT; i++)
        {
            //qDebug()<<"step3";
            if(m_pCameraInfoGroup[i] != NULL && m_pCameraInfoGroup[i]->bIsInit)
            {
                //qDebug()<<"step4";
                m_pCameraGroup[j] = new Camera6467();
                m_pCameraGroup[j]->SetCameraInfo(*(m_pCameraInfoGroup[i]));
                m_pCameraGroup[j]->ConnectToCamera();
                m_pCameraGroup[j]->SetLocalListAndRemoteList(g_pLocalList, g_pRemoteList);
                j++;
                //qDebug()<<"step5";
                qDebug()<<"get camera info, ip = "<<m_pCameraInfoGroup[i]->chIP;
                WriteLog(QString("get camera info, Ip = %1").arg(m_pCameraInfoGroup[i]->chIP));
            }
        }
    }
    else
    {
        qDebug()<<"get camera info failed.";
        WriteLog("get camera info failed.");
    }
    qDebug()<<"out of Thread_Connect_Camera";
    WriteLog("out of Thread_Connect_Camera");
}

void Thread_Connect_Camera::SetCameraAndInfo(Camera6467** pCameraGroup, CameraInfo** pCameraInfoGroup)
{
    if(NULL != pCameraGroup)
         m_pCameraGroup = pCameraGroup;

    if(NULL != pCameraInfoGroup)
        m_pCameraInfoGroup = pCameraInfoGroup;
}

void Thread_Connect_Camera::SetLocalAndRemoteList(CustomResultList *pLocalList, CustomResultList *pRemoteList)
{
    if(NULL != pLocalList)
        g_pLocalList = pLocalList;
    if(NULL != pRemoteList)
        g_pRemoteList = pRemoteList;
}

void Thread_Connect_Camera::InitParameter()
{
    QString qstrCurrentPath = QDir::currentPath();
    qstrCurrentPath.append("/XLW_Config.ini");
    QSettings App_cfg(qstrCurrentPath,QSettings::IniFormat );

    m_dbInfo.qstrConnectName = QString("Thread_Local");
    m_dbInfo.qstrHostName = App_cfg.value("LocalDataBase/HostName").toString();
    m_dbInfo.qstrDataBaseName= App_cfg.value("LocalDataBase/DBName").toString();
    m_dbInfo.qstrUserName= App_cfg.value("LocalDataBase/UserName").toString();
    m_dbInfo.qstrPassWord = App_cfg.value("LocalDataBase/Password").toString();
    m_dbInfo.iDBPort = App_cfg.value("LocalDataBase/Port",50001).toInt();
}

bool Thread_Connect_Camera::WriteLog(const QString qstrLogBuff)
{
    if(qstrLogBuff.isEmpty())
        return false;
    QDir tempDir;
    QString qstrCurrentPath = QDir::currentPath();
    QDateTime currentDateTime = QDateTime::currentDateTime();

    qstrCurrentPath.append("/SNW_log/");
    qstrCurrentPath.append(QString("%1-%2-%3/Thread/").arg(currentDateTime.date().year()).arg(currentDateTime.date().month()).arg(currentDateTime.date().day()));
    if(!tempDir.exists(qstrCurrentPath))
    {
        tempDir.mkdir(qstrCurrentPath);
    }


    QString qstrFileName = QString(qstrCurrentPath+"//Thread_DB_InitCameraInfo_log_%1.log").arg(currentDateTime.toString("yyyy-MM-dd"));

    QFile logFile(qstrFileName);
    if(!logFile.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug()<<"open log File failed";
        return false;
    }
    QString qstrLog = QString("%1: %2").arg(currentDateTime.toString("yyyy-MM-dd hh:mm:ss zzz")).arg(qstrLogBuff);

    QTextStream qStreamLog(&logFile);
    qStreamLog<<qstrLog<<endl;

    logFile.close();
    return true;
}
