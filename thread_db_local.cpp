#include "thread_db_local.h"
#include <QDir>
#include<QDateTime>
#include <QTextStream>


Thread_DB_Local::Thread_DB_Local(QString ThreadName, CustomResultList *pResultList, QObject *parent)
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
    InitParameter();
    m_bExit = false;
}

Thread_DB_Local::~Thread_DB_Local()
{
    StopThread();
    m_pResultList = NULL;
}

void Thread_DB_Local::run()
{
    DBContrl tempDbContrl(m_dbInfo);
    tempDbContrl.ConnetToDB();
    while(true)
    {
        if(m_bExit)
            break;

//        if(!tempDbContrl.IsConnect())
//        {
//            tempDbContrl.ConnetToDB();
//        }
        CameraResult* ptempResult= NULL;
        ptempResult = m_pResultList->GetOneResult();

//        if(NULL !=  ptempResult)
//        {
//            qDebug()<<"Localdb:\n chDevceID:"<<ptempResult->chDeviceID
//                   <<"\n chDevceIp:"<<ptempResult->chDeviceIp
//                  <<"\n chLaneID:"<<ptempResult->chLaneID
//                 <<"\n chListNo:"<<ptempResult->chListNo
//                <<"\n chPlateColor:"<<ptempResult->chPlateColor
//               <<"\n chPlateNO:"<<ptempResult->chPlateNO
//              <<"\n chPlateTime:"<<ptempResult->chPlateTime
//             <<"\n chSignStationID:" <<ptempResult->chSignStationID
//            <<"\n chSignStationName:"  <<ptempResult->chSignStationName
//            <<"\n dw64TimeMS:"   <<ptempResult->dw64TimeMS
//            <<"\n iAreaNo:"    <<ptempResult->iAreaNo
//            <<"\n iDeviceID:"     <<ptempResult->iDeviceID
//            <<"\n iDirection:"      <<ptempResult->iDirection
//            <<"\n iLaneNo:"       <<ptempResult->iLaneNo
//            <<"\n iPlateColor:"        <<ptempResult->iPlateColor
//            <<"\n iPlateTypeNo:"        <<ptempResult->iPlateTypeNo
//            <<"\n iResultNo:"         <<ptempResult->iResultNo
//            <<"\n iRoadNo:"          <<ptempResult->iRoadNo
//            <<"\n iSpeed:"           <<ptempResult->iSpeed
//            <<"\n iVehBodyColorNo:"  <<ptempResult->iVehBodyColorNo
//            <<"\n iVehBodyDeepNo:"  <<ptempResult->iVehBodyDeepNo
//            <<"\n iVehLenth:" <<ptempResult->iVehLenth
//            <<"\n iVehTypeNo:"   <<ptempResult->iVehTypeNo
//            <<"\n pcAppendInfo:"   <<ptempResult->pcAppendInfo;
//        }

        if(NULL !=  ptempResult)
        {
            int iRet =tempDbContrl.InsertNormalData_Local(ptempResult);
            if(0 == iRet)
            {
                tempDbContrl.InsertBigImg_Local(ptempResult->CIMG_FullImage.pbImgData, ptempResult->chListNo, ptempResult->CIMG_FullImage.dwImgSize);
                tempDbContrl.InsertSmallImg_Local(ptempResult->CIMG_PlateImage.pbImgData, ptempResult->chListNo, ptempResult->CIMG_PlateImage.dwImgSize);
                QString qstrLog = QString("wast insert success, list No = %1").arg(ptempResult->chListNo);
                WriteLog(qstrLog);
                qDebug()<<qstrLog;
                emit Signal_DBConnectingChange(true);
            }
            else if(-1 == iRet)
            {
                //TODO 这里应该区分一下插入数据库失败的原因，当因为网络问题而插入失败时，才缓存结果，其他原因时，由于本身结果就损坏了，所以不应该进行缓存
                emit Signal_DBConnectingChange(false);
                QString qstrLog = QString("The DataBase is not connect, Local wast insert failed, save buffer to disk, list No = %1, iRet = %2").arg(ptempResult->chListNo).arg(iRet);
                WriteLog(qstrLog);
                qDebug()<<qstrLog;
                QString qstrFilePath = QString("%1/%2.ini").arg(m_qstrCachePath).arg(ptempResult->chListNo);
                SerializationAsConfigToDisk(qstrFilePath.toStdString().c_str(), *ptempResult);
            }
        }
        if(ptempResult != NULL)
        {
            delete ptempResult;
            ptempResult = NULL;
        }
        usleep(100);
    }
    qDebug()<<"out of Thread_DB_Local";
    WriteLog("out of Thread_DB_Local");
}

void Thread_DB_Local::StopThread()
{
    m_bExit = true;
}

void Thread_DB_Local::InitParameter()
{
    QString qstrCurrentPath = QDir::currentPath();
    qstrCurrentPath.append("/XLW_Config.ini");
    QSettings App_cfg(qstrCurrentPath,QSettings::IniFormat );

    m_dbInfo.qstrConnectName = m_qstrThreadName;
    m_dbInfo.qstrHostName = App_cfg.value("LocalDataBase/HostName").toString();
    m_dbInfo.qstrDataBaseName= App_cfg.value("LocalDataBase/DBName").toString();
    m_dbInfo.qstrUserName= App_cfg.value("LocalDataBase/UserName").toString();
    m_dbInfo.qstrPassWord = App_cfg.value("LocalDataBase/Password").toString();
    m_dbInfo.iDBPort = App_cfg.value("LocalDataBase/Port",50001).toInt();

    m_qstrCachePath = App_cfg.value("LocalDataBase/CachePath","./Local_CachePath/").toString();
}

bool Thread_DB_Local::WriteLog(const QString qstrLogBuf)
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
