#include "thread_db_savestatus.h"


Thread_DB_SaveStatus::Thread_DB_SaveStatus(QString qstrThreadName, Camera6467 **pCameraList, QObject *parent)
    :QThread(parent)
{
    m_qstrThreadName = qstrThreadName;
    if(pCameraList != NULL)
    {
        m_pCameraList = pCameraList;
    }
    else
    {
        m_pCameraList = NULL;
    }
    InitParameter();
    m_bExit = false;    
}

Thread_DB_SaveStatus::~Thread_DB_SaveStatus()
{
    StopThread();
    m_pCameraList = NULL;
}

void Thread_DB_SaveStatus::run()
{
    DBContrl db_Local(m_dbInfo_Local);
    DBContrl db_Remote(m_dbInfo_Remote);
    if(m_bUpLocalDB)
    {
        db_Local.ConnetToDB();
    }
    if(m_bUpRemoteDB)
    {
        db_Remote.ConnetToDB();
    }

    while(true)
    {
        if(m_bExit)
            break;

        Camera6467* pCamera = NULL;
        Camera6467** pCamGroup =m_pCameraList;
        if(NULL == pCamGroup)
            continue;

        for(int i = 0; i < MAX_CAMERA_COUNT; i++)
        {
            pCamera = (*pCamGroup);
            if(NULL == pCamera)
                break;

            int iStatus = pCamera->GetCamStatus();
            int iDeviceID = pCamera->GetDeviceID();
            if(m_bSynTimeEnable)
            {
                pCamera->SynTime();
            }
            //int iDeviceID = 1;
            char chCameraStatus[50] = {0};
            if(0 != iStatus)
            {
                sprintf(chCameraStatus, "Disconnect.");
            }
            else
            {
                sprintf(chCameraStatus, "Connecting.");
            }
            char chCreateTime[128] = {0};
            sprintf(chCreateTime, "%s", QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz").toStdString().c_str());

            char chListNo[260] = {0};
            sprintf(chListNo, "%lld", QDateTime::currentDateTime().toMSecsSinceEpoch());

            //qDebug()<<"listNo = "<<chListNo<<"iDeviceID = "<<iDeviceID<<"CameraStatus="<<chCameraStatus<<"CreateTime= "<<chCreateTime<<"Status = "<<iStatus;

            if(m_bUpLocalDB)
            {
                db_Local.InsertSatus_Local(chListNo, iDeviceID, chCameraStatus, chCreateTime, iStatus);
            }
            if(m_bUpRemoteDB)
            {
                db_Remote.InsertSatus_Remote(chListNo, iDeviceID, chCameraStatus, chCreateTime, iStatus);
            }
            pCamGroup++;
        }
        qDebug()<<"save status keep going";

        sleep(30);
    }
    qDebug()<<"out of save status thread. ";
    WriteLog("out of save status thread. ");
}

void Thread_DB_SaveStatus::StopThread()
{
    m_bExit = true;
}

void Thread_DB_SaveStatus::InitParameter()
{
    QString qstrCurrentPath = QDir::currentPath();
    qstrCurrentPath.append("/XLW_Config.ini");
    QSettings App_cfg(qstrCurrentPath,QSettings::IniFormat );

    m_bUpLocalDB = App_cfg.value("LocalDataBase/Enable").toBool();

    m_dbInfo_Local.qstrConnectName = m_qstrThreadName+QString("_Local");
    m_dbInfo_Local.qstrHostName = App_cfg.value("LocalDataBase/HostName").toString();
    m_dbInfo_Local.qstrDataBaseName= App_cfg.value("LocalDataBase/DBName").toString();
    m_dbInfo_Local.qstrUserName= App_cfg.value("LocalDataBase/UserName").toString();
    m_dbInfo_Local.qstrPassWord = App_cfg.value("LocalDataBase/Password").toString();
    m_dbInfo_Local.iDBPort = App_cfg.value("LocalDataBase/Port",50001).toInt();


    m_bUpRemoteDB = App_cfg.value("RemoteDataBase/Enable").toBool();

    m_dbInfo_Remote.qstrConnectName = m_qstrThreadName+QString("_Remote");
    m_dbInfo_Remote.qstrHostName = App_cfg.value("RemoteDataBase/HostName").toString();
    m_dbInfo_Remote.qstrDataBaseName= App_cfg.value("RemoteDataBase/DBName").toString();
    m_dbInfo_Remote.qstrUserName= App_cfg.value("RemoteDataBase/UserName").toString();
    m_dbInfo_Remote.qstrPassWord = App_cfg.value("RemoteDataBase/Password").toString();
    m_dbInfo_Remote.iDBPort = App_cfg.value("RemoteDataBase/Port",50001).toInt();

    m_bSynTimeEnable = App_cfg.value("ApplicationInfo/SynTimeEnable", true).toBool();
}

bool Thread_DB_SaveStatus::WriteLog(const QString qstrLogBuf)
{
    if(qstrLogBuf.isEmpty())
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


    QString qstrFileName = QString(qstrCurrentPath+"//Thread_DB_Status_log_%1.log").arg(currentDateTime.toString("yyyy-MM-dd"));

    QFile logFile(qstrFileName);
    if(!logFile.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug()<<"open log File failed";
        return false;
    }
    QString qstrLog = QString("%1: %2").arg(currentDateTime.toString("yyyy-MM-dd hh:mm:ss zzz")).arg(qstrLogBuf);

    QTextStream qStreamLog(&logFile);
    qStreamLog<<qstrLog<<endl;

    logFile.close();
    return true;
}
