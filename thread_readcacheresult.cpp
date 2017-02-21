#include"thread_readcacheresult.h"
#include<QSettings>
#include<QDir>
#include<QDateTime>
#include<QTextStream>
#include"CameraResult.h"
#include<QDebug>


Thread_ReadCacheResult::Thread_ReadCacheResult(QString threadName, QObject *parent)
    :QThread(parent)
{
    m_qstrThreadName = threadName;
    m_pReultList = NULL;
    m_iResultType = 0;
    m_bExit = false;
    m_bIsUpLoad = false;
}

Thread_ReadCacheResult::~Thread_ReadCacheResult()
{
    m_pReultList = NULL;
    m_bExit = false;
}

void Thread_ReadCacheResult::SetResutlListAndType(CustomResultList *pResultList, int ReultType)
{
    if(pResultList != NULL)
    {
        m_pReultList = pResultList;
    }
    m_iResultType = ReultType;
    InitParameter();
}

void Thread_ReadCacheResult::run()
{
    qDebug()<<"Thread_ReadCacheResult"<<m_qstrCachePath;
    if(m_qstrCachePath.isEmpty())
        return;
    QDir BufferDir(m_qstrCachePath);
    QRegExp fileRegExp("*.ini");
    QString tempFileName;
    fileRegExp.setPatternSyntax(QRegExp::Wildcard);
    forever
    {
        if(m_bExit)
            break;

        if(NULL == m_pReultList || !m_bIsUpLoad)
        {
            sleep(1);
            continue;
        }

        if(m_pReultList->GetCount() < MAX_RESULT_COUNT)
        {
            QFileInfoList tempInfoList = BufferDir.entryInfoList(QDir::Files);
            QFileInfo firstFileInfo;
            for(QFileInfoList::const_iterator FileBegin = tempInfoList.cbegin(); FileBegin != tempInfoList.cend(); FileBegin++)
            {
                if(fileRegExp.exactMatch((*FileBegin).fileName()))
                {
                    firstFileInfo = (*FileBegin);
                    break;
                }
            }
            if(firstFileInfo.fileName().isEmpty())
            {
                sleep(1);
                continue;
            }
            CameraResult* ptempResult = new CameraResult();
            if(ptempResult == NULL)
            {
                sleep(1);
                continue;
            }
            tempFileName = m_qstrCachePath + firstFileInfo.fileName();

            QString qstrLog = QString("Read cache and find file : %1").arg(tempFileName);
            WriteLog(qstrLog);
            qDebug()<<qstrLog ;
            //SerializationFromDisk(tempFileName.toStdString().c_str(), *tempResult);
            SerializationAsConfigFromDisk(tempFileName.toStdString().c_str(), *ptempResult);
            //            qDebug()<<"\n chDevceID:"<<ptempResult->chDeviceID
            //                   <<"\n chDevceIp:"<<ptempResult->chDeviceIp
            //                  <<"\n chLaneID:"<<ptempResult->chLaneID
            //                 <<"\n chListNo:"<<ptempResult->chListNo
            //                <<"\n chPlateColor:"<<ptempResult->chPlateColor
            //               <<"\n chPlateNO:"<<ptempResult->chPlateNO
            //               <<"\n chPlateTime:"<<ptempResult->chPlateTime
            //              <<"\n chSignStationID:" <<ptempResult->chSignStationID
            //             <<"\n chSignStationName:"  <<ptempResult->chSignStationName
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

            int iRet = m_pReultList->PutOneResult(ptempResult);
            if(iRet == 0)
            {
                //QFile tempBufferFile(firstFileInfo.fileName());
                QFile tempBufferFile(tempFileName);
                tempBufferFile.remove();
                qDebug()<<"add to the List";

                ptempResult = NULL;
            }
            else
            {
                if(NULL != ptempResult)
                {
                    delete ptempResult;
                    ptempResult = NULL;
                }
                qDebug()<<"failed to add to List.";
            }
        }
        else
        {
            WriteLog("the result list is full , stop reading cache.");
        }
        sleep(1);
    }
    qDebug()<<"out of Thread_ReadCacheResult.";
    WriteLog("out of Thread_ReadCacheResult. ");
}

void Thread_ReadCacheResult::StopThread()
{
    m_bExit = true;
    m_pReultList = NULL;
}

void Thread_ReadCacheResult::InitParameter()
{
    QString qstrCurrentPath = QDir::currentPath();
    qstrCurrentPath.append("/XLW_Config.ini");
    QSettings App_cfg(qstrCurrentPath,QSettings::IniFormat );

    if(m_iResultType == 1)
    {
        //读取本地结果缓存信息
        m_qstrCachePath = App_cfg.value("LocalDataBase/CachePath","./Local_CachePath/").toString();
    }
    else if(m_iResultType == 2)
    {
        //读取远程结果缓存信息
        m_qstrCachePath = App_cfg.value("RemoteDataBase/CachePath", "./Remote_Cache/").toString();
    }
}

bool Thread_ReadCacheResult::WriteLog(const QString qstrLogBuf)
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

void Thread_ReadCacheResult::slot_ChangeUpLoadFlag(bool FlagStatus)
{
    qDebug()<<"receive ChangeUpLoadFlag signal value="<<FlagStatus;
    m_bIsUpLoad = FlagStatus;
}
