#include "thread_db_remote.h"


Thread_DB_Remote::Thread_DB_Remote(QString ThreadName, CustomResultList *pResultList, QObject *parent)
    :QThread(parent)
{
    m_qstrThreadName = ThreadName;
    if(pResultList != NULL)
    {
        m_pResultList = pResultList;
    }
    else
    {
        m_pResultList = NULL;
    }
    m_bExit = false;
    InitParameter();
}

Thread_DB_Remote::~Thread_DB_Remote()
{
    StopThread();
    m_pResultList = NULL;
}

void Thread_DB_Remote::run()
{
    DBContrl tempDbContrl_Remote(m_dbInfo);
    tempDbContrl_Remote.ConnetToDB();
    while(true)
    {
        if(m_bExit)
            break;

        CameraResult* ptempResult= NULL;
        ptempResult = m_pResultList->GetOneResult();
        if(NULL !=  ptempResult)
        {
            int iRet = tempDbContrl_Remote.InsertNormalData_Remote(ptempResult);
            if(0 == iRet)
            {
                QString qstrLog = QString("Remote waste insert success,  list No = %1").arg(ptempResult->chListNo);
                WriteLog(qstrLog);
                emit Signal_DBConnectingChange(true);
            }
            else if(-1 == iRet)
            {
                emit Signal_DBConnectingChange(false);
                QString qstrLog = QString("The DateBase is not connect, remote wast insert failed, save buffer to disk, list No = %1").arg(ptempResult->chListNo);
                qDebug()<<qstrLog;
                WriteLog(qstrLog);
                QString qstrFilePath = QString("%1/%2.ini").arg(m_qstrCachePath).arg(ptempResult->chListNo);
                SerializationAsConfigToDisk(qstrFilePath.toStdString().c_str(), *ptempResult);
            }
            delete ptempResult;
            ptempResult = NULL;
        }

        usleep(50);
    }
    qDebug()<<"out of Thread_DB_Remote";
    WriteLog("out of Thread_DB_Remote");
}

void Thread_DB_Remote::StopThread()
{
    m_bExit = true;
}

void Thread_DB_Remote::InitParameter()
{
    QString qstrCurrentPath = QDir::currentPath();
    qstrCurrentPath.append("/XLW_Config.ini");
    QSettings App_cfg(qstrCurrentPath,QSettings::IniFormat );

    m_dbInfo.qstrConnectName = m_qstrThreadName;
    m_dbInfo.qstrHostName = App_cfg.value("RemoteDataBase/HostName").toString();
    m_dbInfo.qstrDataBaseName= App_cfg.value("RemoteDataBase/DBName").toString();
    m_dbInfo.qstrUserName= App_cfg.value("RemoteDataBase/UserName").toString();
    m_dbInfo.qstrPassWord = App_cfg.value("RemoteDataBase/Password").toString();
    m_dbInfo.iDBPort = App_cfg.value("RemoteDataBase/Port",50001).toInt();

    m_qstrCachePath = App_cfg.value("RemoteDataBase/CachePath", "./Remote_Cache/").toString();
}

bool Thread_DB_Remote::WriteLog(const QString qstrLogBuf)
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


    QString qstrFileName = QString(qstrCurrentPath+"//Thread_%1_log_%2.log").arg(m_qstrThreadName).arg(currentDateTime.toString("yyyy-MM-dd"));

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
