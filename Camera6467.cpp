#include "Camera6467.h"
#include "HvDevice/HvDeviceNew.h"
#include"HvDevice/swerror.h"
#include "tinyxml/tinyxml.h"
#include<math.h>
#include <QDir>
#include <QTextStream>
#include <QDate>
#include <QTime>
#include <QDebug>
#include <QSettings>

Camera6467::Camera6467()
{
    m_CameraResult = NULL;
    m_BufferResult = NULL;
    m_hHvHandle = NULL;
    m_hWnd = NULL;
    m_pThreadMoveResult = NULL;
    g_pResultList_Local = NULL;
    g_pResultList_Remote = NULL;

    m_pResultList = new CustomResultList();
    m_strIP = std::string("");
    m_iCompressQuality = 20;
    m_bResultComplete = false;
    m_iDeviceID = 0;
    //合成图片初始化
    //GetEncoderClsid(L"image/jpeg", &m_jpgClsid);
    //GetEncoderClsid(L"image/bmp", &m_bmpClsid);

    ReadConfig();
}

Camera6467::Camera6467( const char* chIP, HWND*  hWnd, int Msg)
{
    if(NULL != chIP && NULL != hWnd)
    {
        m_strIP = std::string(chIP);
        m_hWnd = hWnd;
        m_iMsg = Msg;
    }
    else
    {
        m_hWnd = NULL;
        m_iMsg = Msg;
    }
    m_hHvHandle = NULL;
    m_iCompressQuality = 20;
    m_CameraResult = NULL;
    m_BufferResult = NULL;
    m_pThreadMoveResult = NULL;
    g_pResultList_Local = NULL;
    g_pResultList_Remote = NULL;

    m_pResultList = new CustomResultList();

    m_bResultComplete = false;
    //合成图片初始化
    //	GetEncoderClsid(L"image/jpeg", &m_jpgClsid);
    //	GetEncoderClsid(L"image/bmp", &m_bmpClsid);

    ReadConfig();

}

Camera6467::~Camera6467()
{
    if(NULL != m_hHvHandle)
    {
        HVAPI_CloseEx((HVAPI_HANDLE_EX)m_hHvHandle);
        m_hHvHandle = NULL;
    }
    if(NULL != m_CameraResult)
    {
        delete m_CameraResult;
        m_CameraResult = NULL;
    }
    if (NULL != m_BufferResult)
    {
        delete m_BufferResult;
        m_BufferResult = NULL;
    }
    if(NULL != m_pThreadMoveResult)
    {
        m_pThreadMoveResult->StopThread();
        delete m_pThreadMoveResult;
        m_pThreadMoveResult = NULL;
    }
    if(NULL != m_pResultList)
    {
        m_pResultList->ClearAllResult();
        delete m_pResultList;
        m_pResultList = NULL;
    }
    if(NULL != g_pResultList_Local)
        g_pResultList_Local = NULL;
    if(NULL != g_pResultList_Remote)
        g_pResultList_Remote = NULL;

    m_bResultComplete = false;
    m_bExit = true;
    m_hWnd = NULL;
    qDebug()<<"out of camera"<<m_strIP.c_str();
    WriteLog("out of camera");
    m_strIP.clear();
}

void Camera6467::ReadHistoryInfo()
{
    QString qstrCurrentPath = QDir::currentPath();
    QString qstrFilePath = qstrCurrentPath+"/SafeModeConfig.ini";
    QSettings App_cfg(qstrFilePath,QSettings::IniFormat );

    QString qstrBeginTime, qstrEndTime;
    QString qstrParameterGroupName = QString("%1").arg(m_strIP.c_str());
    App_cfg.beginGroup(qstrParameterGroupName);

    QDateTime qdtime;
    qdtime = QDateTime::currentDateTime();
    QString qstrSaveTime = qdtime.toString("yyyy.MM.dd_hh");

    qstrBeginTime = App_cfg.value("BeginTime",qstrSaveTime).toString();
    qstrEndTime = App_cfg.value("EndTime","").toString();
    sprintf(m_SaveModelInfo.chBeginTime, "%s", qstrBeginTime.toStdString().c_str());
    sprintf(m_SaveModelInfo.chEndTime, "%s", qstrEndTime.toStdString().c_str());
    m_SaveModelInfo.iSafeModeEnable = App_cfg.value("SaveModeEnable",0).toInt();
    m_SaveModelInfo.iIndex = App_cfg.value("Index", 0).toInt();
    m_SaveModelInfo.iDataType = App_cfg.value("DataType", 0).toInt();

    App_cfg.endGroup();
}

void Camera6467::WriteHistoryInfo( SaveModeInfo& SaveInfo )
{
    QString qstrCurrentPath = QDir::currentPath();
    QString qstrFilePath = qstrCurrentPath+"/SafeModeConfig.ini";
    QSettings App_cfg(qstrFilePath,QSettings::IniFormat );

    QString qstrParameterGroupName = QString("%1").arg(m_strIP.c_str());
    App_cfg.beginGroup(qstrParameterGroupName);

    QString qstrBeginTime(SaveInfo.chBeginTime);
    QString qstrEndTime(SaveInfo.chEndTime);

    App_cfg.setValue("BeginTime", qstrBeginTime);
    App_cfg.setValue("EndTime", qstrEndTime);
    App_cfg.setValue("Index", SaveInfo.iIndex);
    App_cfg.setValue("DataType", SaveInfo.iDataType);
    App_cfg.setValue("SaveModeEnable", SaveInfo.iSafeModeEnable);

    App_cfg.endGroup();
}

bool Camera6467::SaveImgToDisk( char* chImgPath, BYTE* pImgData, DWORD dwImgSize )
{
    WriteLog("begin SaveImgToDisk");
    if(NULL == pImgData)
    {
        WriteLog("end1 SaveImgToDisk");
        return false;
    }
    bool bRet = false;
    size_t iWritedSpecialSize = 0;
    std::string tempFile(chImgPath);
    size_t iPosition = tempFile.rfind("\\");
    std::string tempDir1 = tempFile.substr(0, iPosition);

    QDir tempDir;
    // if (MakeSureDirectoryPathExists(tempDir.c_str()))
    if(tempDir.mkpath(QString(tempDir1.c_str())))
    {
        FILE* fp = fopen(chImgPath, "wb");
        if(fp)
        {
            //iWritedSpecialSize = fwrite(pImgData, dwImgSize , 1, fp);
            iWritedSpecialSize = fwrite(pImgData, 1 , dwImgSize, fp);
            fclose(fp);
            fp = NULL;
            bRet = true;
        }
        if (iWritedSpecialSize == dwImgSize)
        {
            char chLogBuff[MAX_PATH] = {0};
            sprintf(chLogBuff, "%s save success", chImgPath);
            WriteLog(chLogBuff);
        }
    }
    else
    {
        char chLogBuff[MAX_PATH] = {0};
        sprintf(chLogBuff, "%s save failed", chImgPath);
        WriteLog(chLogBuff);
        bRet = false;
    }
    WriteLog("end SaveImgToDisk");
    return bRet;
}


bool Camera6467::SetCameraInfo( CameraInfo& camInfo )
{
    m_strIP = std::string(camInfo.chIP);
    m_strDeviceID = std::string(camInfo.chDeviceID);
    sprintf(m_chDeviceID, "%s", camInfo.chDeviceID);
    sprintf(m_chLaneID, "%s", camInfo.chLaneID);
    sprintf(m_chStationID, "%s", camInfo.chStationID);
    m_bLogEnable = camInfo.bLogEnable;
    m_bSynTime = camInfo.bSynTimeEnable;
    m_iDirection = camInfo.iDirection;
    m_iDeviceID = camInfo.iDeviceID;

    return true;
}

int Camera6467::GetCamStatus()
{
    if(NULL == m_hHvHandle)
        return 1;
    DWORD dwStatus = 1;

    if ( HVAPI_GetConnStatusEx((HVAPI_HANDLE_EX)m_hHvHandle, CONN_TYPE_RECORD, &dwStatus) == S_OK )
    {
        if (dwStatus == CONN_STATUS_NORMAL
                || dwStatus == CONN_STATUS_RECVDONE)
        {
            m_iConnectStatus = 0;
        }
        else if (dwStatus == CONN_STATUS_RECONN)
        {
            m_iConnectStatus = 1;
        }
        else
        {
            m_iConnectStatus = 1;
        }
    }
    else
    {
        m_iConnectStatus = 1;
    }
    return m_iConnectStatus;
}

int Camera6467::GetDeviceID()
{
    return m_iDeviceID;
}

char* Camera6467::GetStationID()
{
    return m_chStationID;
}

//char* Camera6467::GetDeviceID()
//{
//    return m_chDeviceID;
//}

char* Camera6467::GetLaneID()
{
    return m_chLaneID;
}

const char* Camera6467::GetCameraIP()
{
    return m_strIP.c_str();
}

CameraResult* Camera6467::GetOneResult()
{
    CameraResult* ptempResult = NULL;
    //    ptempResult = m_pResultList->GetOneResult();
    //    return ptempResult;

    if (NULL != m_BufferResult && m_bResultComplete)
    {
        ptempResult = new CameraResult(*m_BufferResult);
    }
    return ptempResult;
}

bool Camera6467::ConnectToCamera()
{
    if(m_strIP.empty())
    {
        WriteLog("ConnectToCamera:: please finish the camera ip address");
        return false;
    }
    if(NULL != m_hHvHandle)
    {
        HVAPI_CloseEx((HVAPI_HANDLE_EX)m_hHvHandle);
        m_hHvHandle = NULL;
    }
    m_hHvHandle = HVAPI_OpenEx(m_strIP.c_str(), NULL);
    if(NULL == m_hHvHandle)
    {
        WriteLog("ConnectToCamera:: Open CameraHandle failed!");
        return false;
    }

    ReadHistoryInfo();
    char chCommand[1024] = {0};
    sprintf(chCommand, "DownloadRecord,BeginTime[%s],Index[%d],Enable[%d],EndTime[%s],DataInfo[%d]", m_SaveModelInfo.chBeginTime, m_SaveModelInfo.iIndex, m_SaveModelInfo.iSafeModeEnable, m_SaveModelInfo.chEndTime, m_SaveModelInfo.iDataType);

    WriteLog(chCommand);

    if ( (HVAPI_SetCallBackEx(m_hHvHandle, (PVOID)RecordInfoBeginCallBack, this, 0, CALLBACK_TYPE_RECORD_INFOBEGIN, /*NULL*/chCommand) != S_OK) ||
         (HVAPI_SetCallBackEx(m_hHvHandle, (PVOID)RecordInfoPlateCallBack, this, 0, CALLBACK_TYPE_RECORD_PLATE, /*NULL*/chCommand) != S_OK) ||
         (HVAPI_SetCallBackEx(m_hHvHandle, (PVOID)RecordInfoBigImageCallBack, this, 0, CALLBACK_TYPE_RECORD_BIGIMAGE, /*NULL*/chCommand) != S_OK) ||
         (HVAPI_SetCallBackEx(m_hHvHandle, (PVOID)RecordInfoSmallImageCallBack, this, 0, CALLBACK_TYPE_RECORD_SMALLIMAGE, /*NULL*/chCommand) != S_OK) ||
         (HVAPI_SetCallBackEx(m_hHvHandle, (PVOID)RecordInfoBinaryImageCallBack, this, 0, CALLBACK_TYPE_RECORD_BINARYIMAGE, /*NULL*/chCommand) != S_OK) ||
         (HVAPI_SetCallBackEx(m_hHvHandle, (PVOID)RecordInfoEndCallBack, this, 0, CALLBACK_TYPE_RECORD_INFOEND, /*NULL*/chCommand) != S_OK)
         )
    {
        WriteLog("ConnectToCamera:: SetCallBack failed.");
        qDebug()<<m_strIP.c_str()<<"ConnectToCamera:: SetCallBack failed.";
        HVAPI_CloseEx(m_hHvHandle);
        m_hHvHandle = NULL;
        m_dwConnectStatus = -1;
        return false;
    }
    else
    {
        m_dwConnectStatus = 0;
        WriteLog("ConnectToCamera:: SetCallBack success.");
        qDebug()<<"ConnectToCamera:: SetCallBack success.";
    }

    if((m_bSaveDB_Local || m_bSaveDB_Remote) && NULL ==m_pThreadMoveResult)
    {
        m_pThreadMoveResult = new Thread_MoveResultToDBList(this);
        m_pThreadMoveResult->start();
        qDebug()<<"create move result thread.";
    }
    return true;
}

void Camera6467::ReadConfig()
{
    QString qstrCurrentPath = QDir::currentPath();
    QString qstrFilePath = qstrCurrentPath+"/XLW_Config.ini";
    QSettings App_cfg(qstrFilePath,QSettings::IniFormat );
    m_qstrBackupPath = App_cfg.value("ApplicationInfo/BackupResultSavePath",qstrCurrentPath.append("/BackupResult/") ).toString();
    m_qstrResultPath = App_cfg.value("ApplicationInfo/ResultSavePath",qstrCurrentPath.append("/Result/") ).toString();
    m_iCompressQuality = App_cfg.value("ApplicationInfo/CompressQuality",-1 ).toInt();
    m_bLogEnable = App_cfg.value("ApplicationInfo/LogEnable").toBool();
    m_bSaveDB_Local = App_cfg.value("LocalDataBase/Enable").toBool();
    m_bSaveDB_Remote = App_cfg.value("RemoteDataBase/Enable").toBool();

    m_qstrLocalCachePath = App_cfg.value("LocalDataBase/CachePath","./Local_CachePath/").toString();
    m_qstrRemoteCachePath = App_cfg.value("RemoteDataBase/CachePath", "./Remote_Cache/").toString();
}

bool Camera6467::WriteLog( const char* chlog )
{
    if(!m_bLogEnable || NULL == chlog)
        return false;

    QDir tempDir;
    QString qstrCurrentPath = QDir::currentPath();
    QDateTime currentDateTime = QDateTime::currentDateTime();

    qstrCurrentPath.append("/SNW_log/");
    qstrCurrentPath.append(QString("%1-%2-%3").arg(currentDateTime.date().year()).arg(currentDateTime.date().month()).arg(currentDateTime.date().day()));
    qstrCurrentPath.append(QString("/%1/").arg(QString(m_strIP.c_str())) );
    if(!tempDir.exists(qstrCurrentPath))
    {
        tempDir.mkpath(qstrCurrentPath);
    }

    QString qstrFileName = QString(qstrCurrentPath+"/Camera_log_%1.log").arg(currentDateTime.toString("yyyy-MM-dd"));

    m_qMutex_Log.lock();
    QFile logFile(qstrFileName);
    if(!logFile.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug()<<"open log File failed";
        m_qMutex_Log.unlock();
        return false;
    }
    QString qstrLog = QString("%1 : %2").arg(currentDateTime.toString("yyyy-MM-dd hh:mm:ss zzz")).arg(QString(chlog));

    QTextStream qStreamLog(&logFile);
    qStreamLog<<qstrLog<<endl;
    logFile.close();

    m_qMutex_Log.unlock();
    return true;
}

bool Camera6467::TakeCapture()
{
    if (NULL == m_hHvHandle)
        return false;

    bool bRet = true;
    char chRetBuf[1024] = {0};
    int nRetLen = 0;

    if (HVAPI_ExecCmdEx(m_hHvHandle, "SoftTriggerCapture", chRetBuf, 1024, &nRetLen) != S_OK)
    {
        bRet = false;
        char chCaptureLog3[MAX_PATH] = {0};
        sprintf(chCaptureLog3, "Camera: %s SoftTriggerCapture failed", m_strIP.c_str());
        WriteLog(chCaptureLog3);
    }
    else
    {
        char chCaptureLog4[MAX_PATH] = {0};
        sprintf(chCaptureLog4, "Camera: %s SoftTriggerCapture success", m_strIP.c_str());
        WriteLog(chCaptureLog4);
    }
    return bRet;
}

bool Camera6467::SynTime()
{
    if (NULL == m_hHvHandle || m_dwConnectStatus != 0)
        return false;
    if (!m_bSynTime)
    {
        return false;
    }
    WriteLog("SynTime begin");

    //    static time_t starttime = time(NULL);
    //    static DWORD starttick = GetTickCount();
    //    DWORD dwNowTick = GetTickCount() - starttick;
    //    time_t nowtime = starttime + (time_t)(dwNowTick / 1000);
    //    struct tm *pTM = localtime(&nowtime);
    //    DWORD dwMS = dwNowTick % 1000;

    //    char chTemp[256]={ 0 };
    //    sprintf(chTemp, "SetTime,Date[%d.%02d.%02d],Time[%02d:%02d:%02d]",
    //            pTM->tm_year + 1900,  pTM->tm_mon, pTM->tm_mday,
    //            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);
    //    WriteLog(chTemp);

    QDate currentDate = QDate::currentDate();
    QTime currentTime = QTime::currentTime();
    char chTemp[256]={ 0 };
    sprintf(chTemp, "SetTime,Date[%d.%02d.%02d],Time[%02d:%02d:%02d]",
            currentDate.year(), currentDate.month(), currentDate.day(),
            currentTime.hour(), currentTime.minute(), currentTime.second());

    char szRetBuf[1024] = {0};
    int nRetLen = 0;
    if (m_hHvHandle != NULL)
    {
        try
        {
            if (HVAPI_ExecCmdEx(m_hHvHandle, chTemp, szRetBuf, 1024, &nRetLen) != S_OK)
            {
                char chSynTimeLogBuf1[MAX_PATH] = {0};
                sprintf(chSynTimeLogBuf1, "Camera: %s SynTime failed", m_strIP.c_str());
                WriteLog(chSynTimeLogBuf1);
                return false;
            }
            else
            {
                char chSynTimeLogBuf2[MAX_PATH] = {0};
                sprintf(chSynTimeLogBuf2, "Camera: %s SynTime success", m_strIP.c_str());
                WriteLog(chSynTimeLogBuf2);
            }
        } catch (...)
        {
            char chSynTimeLogBuf3[MAX_PATH] = {0};
            sprintf(chSynTimeLogBuf3, "Camera: %s SynTime take exception", m_strIP.c_str());
            WriteLog(chSynTimeLogBuf3);
        }
    }
    WriteLog("SynTime end");
    return 0;
}

bool Camera6467::SynTime( int Year, int Month, int Day, int Hour, int Minute, int Second, int MilientSecond )
{
    if (NULL == m_hHvHandle || m_dwConnectStatus != 0)
        return false;
    if (!m_bSynTime)
    {
        return false;
    }
    if (abs(Month) > 12 || abs(Day) > 31 || abs(Hour) > 24 || abs(Minute) > 60 || abs(Second) > 60)
    {
        WriteLog("SynTime  failed: time value is invalid.");
        return false;
    }
    WriteLog("SynTime begin");


    char chTemp[256]={ 0 };
    sprintf(chTemp, "SetTime,Date[%d.%02d.%02d],Time[%02d:%02d:%02d]",
            abs(Year),  abs(Month), abs(Day),
            abs(Hour), abs(Minute), abs(Second));
    WriteLog(chTemp);
    char szRetBuf[1024] = {0};
    int nRetLen = 0;
    if (m_hHvHandle != NULL)
    {
        try
        {
            if (HVAPI_ExecCmdEx(m_hHvHandle, chTemp, szRetBuf, 1024, &nRetLen) != S_OK)
            {
                char chSynTimeLogBuf1[MAX_PATH] = {0};
                sprintf(chSynTimeLogBuf1, "Camera: %s SynTime failed", m_strIP.c_str());
                WriteLog(chSynTimeLogBuf1);
                return false;
            }
            else
            {
                char chSynTimeLogBuf2[MAX_PATH] = {0};
                sprintf(chSynTimeLogBuf2, "Camera: %s SynTime success", m_strIP.c_str());
                WriteLog(chSynTimeLogBuf2);
            }
        } catch (...)
        {
            char chSynTimeLogBuf3[MAX_PATH] = {0};
            sprintf(chSynTimeLogBuf3, "Camera: %s SynTime take exception", m_strIP.c_str());
            WriteLog(chSynTimeLogBuf3);
        }
    }
    WriteLog("SynTime end");
    return true;
}

void Camera6467::AnalysisAppendInfo( CameraResult* record )
{
    sprintf(record->chListNo, "%lld", QDateTime::currentMSecsSinceEpoch());

    if (strstr(record->chPlateNO, "无车牌"))
    {
        record->iPlateColor = 0;
        record->iPlateTypeNo=0;
    }
    else
    {
        char tmpPlateColor[20] = {0};
        strncpy(tmpPlateColor, record->chPlateNO,2);
        strncpy(record->chPlateColor, record->chPlateNO,2);
        if (strstr(tmpPlateColor, "蓝"))
        {
            record->iPlateColor = 3;
            record->iPlateTypeNo=2;
        }
        else if (strstr(tmpPlateColor, "黄"))
        {
            record->iPlateColor = 2;
            record->iPlateTypeNo=1;
        }
        else if (strstr(tmpPlateColor, "黑"))
        {
            record->iPlateColor = 4;
            record->iPlateTypeNo=3;
        }
        else if (strstr(tmpPlateColor, "白"))
        {
            record->iPlateColor = 1;
            record->iPlateTypeNo=6;
        }
        else
        {
            record->iPlateColor = 0;
            record->iPlateTypeNo=0;
        }
        //获取车牌号
        char sztempPlate[20] = {0};
        sprintf(sztempPlate, "%s", record->chPlateNO + 2);
        //sprintf(sztempPlate, "%s", record->chPlateNO);
        if (NULL != sztempPlate)
        {
            memset(record->chPlateNO, 0, sizeof(record->chPlateNO));
            sprintf(record->chPlateNO,"%s",sztempPlate);
        }
    }
    char * pchObj = NULL;
    pchObj = (char *)strstr(record->pcAppendInfo, "车道");
    if (pchObj)
    {
        sscanf(pchObj, "车道:%d", &(record->iLaneNo));
    }
    else
    {
        record->iLaneNo = 0;
    }
    pchObj = (char *)strstr(record->pcAppendInfo, "路口方向");
    if (pchObj)
    {
        sscanf(pchObj, "路口方向:%d", &(record->iDirection));
    }
    else
    {
        record->iDirection = 0;
    }
    if (0 != record->dw64TimeMS)
    {
        QDateTime qTime;
        qTime.setMSecsSinceEpoch(qint64(record->dw64TimeMS));
        QString qstrTime = qTime.toString("yyyy.MM.dd hh:mm:ss.zzz");
        sprintf(record->chPlateTime, "%s", qstrTime.toLocal8Bit().data());
        //CTime tm(record->dw64TimeMS/1000);
        //sprintf(record->chPlateTime, "%04d%02d%02d%02d%02d%02d%03d", tm.GetYear(), tm.GetMonth(), tm.GetDay(), tm.GetHour(), tm.GetMinute(), tm.GetSecond(), record->dw64TimeMS%1000);
    }
    else
    {
        QString qstrTime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz");
        sprintf(record->chPlateTime, "%s", qstrTime.toLocal8Bit().data());
        //		SYSTEMTIME st;
        //		GetLocalTime(&st);
        //		sprintf(record->chPlateTime, "%04d%02d%02d%02d%02d%02d%03d", st.wYear, st.wMonth, st.wDay	,st.wHour, st.wMinute,st.wSecond, st.wMilliseconds);
    }

    //------------车道
    pchObj = (char*)strstr(record->pcAppendInfo, "车辆类型");
    if(pchObj)
    {
        char szCarType[20] = {0};
        sscanf(pchObj, "车辆类型:%s", szCarType);
        if (strstr(szCarType,"大"))
        {
            record->iVehTypeNo=2;
        }
        if (strstr(szCarType,"中"))
        {
            record->iVehTypeNo=2;
        }
        if (strstr(szCarType,"小"))
        {
            record->iVehTypeNo=1;
        }
        if(strstr(szCarType,"客车"))
        {
            record->iVehTypeNo=1;
        }
        if(strstr(szCarType,"货车"))
        {
            record->iVehTypeNo=2;
        }
        if(strstr(szCarType,"牵引车"))
        {
            record->iVehTypeNo=3;
        }
        if(strstr(szCarType,"专项作业车"))
        {
            record->iVehTypeNo=4;
        }
        if(strstr(szCarType,"电车"))
        {
            record->iVehTypeNo=5;
        }
        if(strstr(szCarType,"摩托车"))
        {
            record->iVehTypeNo=6;
        }
        if(strstr(szCarType,"三轮汽车"))
        {
            record->iVehTypeNo=7;
        }
        if(strstr(szCarType,"拖拉机"))
        {
            record->iVehTypeNo=8;
        }
        if(strstr(szCarType,"轮式机械"))
        {
            record->iVehTypeNo=9;
        }
        if(strstr(szCarType,"全挂车"))
        {
            record->iVehTypeNo=10;
        }
        if(strstr(szCarType,"半挂车"))
        {
            record->iVehTypeNo=11;
        }
        if(strstr(szCarType,"其他"))
        {
            record->iVehTypeNo=12;
        }
    }

    //---------------车身颜色
    pchObj= (char*)strstr(record->pcAppendInfo,"车身颜色:");

    record->iVehBodyColorNo=0;
    if(pchObj)
    {
        char szBodyColour[256]={0};
        sscanf(pchObj,"车身颜色:%s",szBodyColour);
        if(strstr(szBodyColour,"白"))
            record->iVehBodyColorNo=1;

        if(strstr(szBodyColour,"灰"))
            record->iVehBodyColorNo=2;

        if(strstr(szBodyColour,"黄"))
            record->iVehBodyColorNo=3;

        if(strstr(szBodyColour,"粉"))
            record->iVehBodyColorNo=4;

        if(strstr(szBodyColour,"红"))
            record->iVehBodyColorNo=5;

        if(strstr(szBodyColour,"紫"))
            record->iVehBodyColorNo=6;

        if(strstr(szBodyColour,"绿"))
            record->iVehBodyColorNo=7;

        if(strstr(szBodyColour,"蓝"))
            record->iVehBodyColorNo=8;

        if(strstr(szBodyColour,"棕"))
            record->iVehBodyColorNo=9;

        if(strstr(szBodyColour,"黑"))
            record->iVehBodyColorNo=10;
    }

    //---------车身深浅度
    pchObj = (char*)(strstr(record->pcAppendInfo,"深浅度:"));
    if(pchObj)
    {
        char szBodyColourDeep[256]={0};
        sscanf(pchObj,"深浅度:%s",szBodyColourDeep);

        if(strstr(szBodyColourDeep,"深"))
            record->iVehBodyDeepNo=1;

        if(strstr(szBodyColourDeep,"浅"))
            record->iVehBodyDeepNo=0;
    }
    //--------------------测速
    pchObj = (char*)(strstr(record->pcAppendInfo,"测速:"));
    if(pchObj)
    {
        int iCarSpeed = 0;
        sscanf(pchObj,"测速:%d Km/h",&iCarSpeed);
        record->iSpeed = iCarSpeed;
    }
}

int Camera6467::RecordInfoBegin( DWORD dwCarID )
{
    char chlogInfo[260] = {0};
    sprintf(chlogInfo, "RecordInfoBegin -begin- dwCarID = %ld", dwCarID);
    WriteLog(chlogInfo);
    m_bResultComplete = false;
    if(NULL != m_CameraResult)
    {
        delete m_CameraResult;
        m_CameraResult = NULL;
    }
    m_CameraResult = new CameraResult();
    sprintf(m_CameraResult->chDeviceIp, "%s", m_strIP.c_str());
    m_CameraResult->dwCarID = dwCarID;
    char chlogInfo2[260] = {0};
    sprintf(chlogInfo2, "RecordInfoBegin -end- dwCarID = %ld", dwCarID);
    WriteLog(chlogInfo2);
    return 0;
}

int Camera6467::RecordInfoEnd( DWORD dwCarID )
{
    char chLog[260] = {0};
    sprintf(chLog, "RecordInfoEnd begin, dwCarID = %ld", dwCarID);
    WriteLog(chLog);
    if (NULL == m_CameraResult)
    {
        return 0;
    }

    //CTime tm(m_CameraResult->dw64TimeMS/1000);
    //SaveModeInfo TempSaveModeInfo;
    //sprintf(TempSaveModeInfo.chBeginTime, "%d.%02d.%02d_%02d", tm.GetYear(), tm.GetMonth(), tm.GetDay(), tm.GetHour());
    //WriteHistoryInfo(TempSaveModeInfo);

    QDateTime qTime;
    qTime.setMSecsSinceEpoch(qint64(m_CameraResult->dw64TimeMS));
    SaveModeInfo TempSaveModeInfo;
    sprintf(TempSaveModeInfo.chBeginTime, "%d.%02d.%02d_%02d", qTime.date().year(),  qTime.date().month(), qTime.date().day(), qTime.time().hour());
    WriteHistoryInfo(TempSaveModeInfo);

    //    m_qMutex_Result.lock();

    //    if (m_ResultList.size() < 100)
    //    {
    //        m_ResultList.push_back(m_CameraResult);
    //    }
    //    else
    //    {
    //        delete m_CameraResult;
    //    }
    //    m_CameraResult = NULL;

    //    m_qMutex_Result.unlock();

    m_bResultComplete = true;

    if (NULL != m_BufferResult)
    {
        delete m_BufferResult;
        m_BufferResult = NULL;
    }
    m_BufferResult = new CameraResult(*m_CameraResult);

    //char chPath[260] = {0};
    //sprintf(chPath, ".\\%lld.jpg", QDateTime::currentMSecsSinceEpoch());
    //SaveImgToDisk(chPath, m_CameraResult->CIMG_FullImage.pbImgData, m_CameraResult->CIMG_FullImage.dwImgSize);
    if( !m_bSaveDB_Local && !m_bSaveDB_Remote)
    {
        qDebug()<<"Save To disk";
        SaveResultToDisk(m_CameraResult);
        delete m_CameraResult;
        m_CameraResult = NULL;
    }
    else
    {
        int iPutResult = m_pResultList->PutOneResult(m_CameraResult);
        if(0 == iPutResult)
        {
            qDebug()<<"Save To list success";
        }
        else
        {
            qDebug()<<"Save To list failed, iPutResult = "<<iPutResult;
            if(m_bSaveDB_Local)
            {
                //缓存一份到本地缓存目录
                QString qstrLocalCacheFilePath = QString("%1/%2.ini").arg(m_qstrLocalCachePath).arg(m_CameraResult->chListNo);
                SerializationAsConfigToDisk(qstrLocalCacheFilePath.toStdString().c_str(), *m_CameraResult);
            }
            if(m_bSaveDB_Remote)
            {
                //缓存一份到远程缓存目录
                QString qstrRemoteCacheFilePath = QString("%1/%2.ini").arg(m_qstrRemoteCachePath).arg(m_CameraResult->chListNo);
                SerializationAsConfigToDisk(qstrRemoteCacheFilePath.toStdString().c_str(), *m_CameraResult);
            }

            delete m_CameraResult;
        }
        m_CameraResult = NULL;
    }
    char chLog2[260] = {0};
    sprintf(chLog2, "RecordInfoEnd end, dwCarID = %ld", dwCarID);
    WriteLog(chLog2);
    return 0;
}

int Camera6467::RecordInfoPlate( DWORD dwCarID, LPCSTR pcPlateNo, LPCSTR pcAppendInfo, DWORD dwRecordType, DWORD64 dw64TimeMS )
{
    m_bResultComplete = false;

    if (NULL == m_CameraResult)
    {
        return -1;
    }
    char chlogInfo[260] = {0};
    sprintf(chlogInfo, "RecordInfoPlate -begin- dwCarID = %ld", dwCarID);
    WriteLog(chlogInfo);
    if (m_CameraResult->dwCarID == dwCarID)
    {
        m_CameraResult->dw64TimeMS = dw64TimeMS;
        sprintf(m_CameraResult->chPlateNO, "%s",pcPlateNo);
        //sprintf(m_CameraResult->chPlateNO, "%s",pcPlateNo+3);
        memset(m_CameraResult->pcAppendInfo, 0, sizeof(m_CameraResult->pcAppendInfo));
        HVAPIUTILS_ParsePlateXmlStringEx(pcAppendInfo, m_CameraResult->pcAppendInfo, 2048);
        WriteLog(QString::fromLocal8Bit(m_CameraResult->chPlateNO).toStdString().c_str());
        WriteLog(QString::fromLocal8Bit(m_CameraResult->pcAppendInfo).toStdString().c_str());
        char qstrPlateTime[260] = {0};
        sprintf(qstrPlateTime, "the plate time is %lld", dw64TimeMS);
        WriteLog(qstrPlateTime);
        AnalysisAppendInfo(m_CameraResult);
    }
    char chlogInfo2[260] = {0};
    sprintf(chlogInfo2, "RecordInfoPlate -end- dwCarID = %ld", dwCarID);
    WriteLog(chlogInfo2);

    qDebug()<<"RecordInfoPlate plateNo = "<<QString::fromLocal8Bit(m_CameraResult->chPlateNO);
    return 0;
}

int Camera6467::RecordInfoBigImage( DWORD dwCarID, WORD wImgType, WORD wWidth, WORD wHeight, PBYTE pbPicData, DWORD dwImgDataLen, DWORD dwRecordType, DWORD64 dw64TimeMS )
{
    m_bResultComplete = false;

    if (NULL == m_CameraResult)
    {
        return -1;
    }
    char chlogInfo[260] = {0};
    sprintf(chlogInfo, "RecordInfoBigImage -begin- dwCarID = %ld", dwCarID);
    WriteLog(chlogInfo);
    if (m_CameraResult->dwCarID == dwCarID)
    {
        if (NULL != m_CameraResult->CIMG_FullImage.pbImgData)
        {
            WriteLog("RecordInfoBigImage SpecialImage  ");
            if(NULL == m_CameraResult->CIMG_SpecialImage.pbImgData)
            {
                delete[] m_CameraResult->CIMG_SpecialImage.pbImgData;
                m_CameraResult->CIMG_SpecialImage.pbImgData = NULL;
            }

            m_CameraResult->CIMG_SpecialImage.pbImgData = new(std::nothrow) BYTE[dwImgDataLen];
            m_CameraResult->CIMG_SpecialImage.dwImgSize = 0;
            if (  NULL != m_CameraResult->CIMG_SpecialImage.pbImgData)
            {
                memcpy(m_CameraResult->CIMG_SpecialImage.pbImgData, pbPicData, dwImgDataLen);
                m_CameraResult->CIMG_SpecialImage.wImgWidth = wWidth;
                m_CameraResult->CIMG_SpecialImage.wImgHeight = wHeight;
                m_CameraResult->CIMG_SpecialImage.dwImgSize = dwImgDataLen;
                m_CameraResult->CIMG_SpecialImage.wImgType = wImgType;
            }
        }
        else
        {
            WriteLog("RecordInfoBigImage FullImage  ");
            m_CameraResult->CIMG_FullImage.pbImgData = new(std::nothrow) BYTE[dwImgDataLen];
            m_CameraResult->CIMG_FullImage.dwImgSize = 0;
            if (  NULL != m_CameraResult->CIMG_FullImage.pbImgData)
            {
                memcpy(m_CameraResult->CIMG_FullImage.pbImgData, pbPicData, dwImgDataLen);
                m_CameraResult->CIMG_FullImage.wImgWidth = wWidth;
                m_CameraResult->CIMG_FullImage.wImgHeight = wHeight;
                m_CameraResult->CIMG_FullImage.dwImgSize = dwImgDataLen;
                m_CameraResult->CIMG_FullImage.wImgType = wImgType;
            }
        }
    }
    char chlogInfo2[260] = {0};
    sprintf(chlogInfo2, "RecordInfoBigImage -end- dwCarID = %ld",dwCarID);
    WriteLog(chlogInfo2);
    return 0;
}

int Camera6467::RecordInfoSmallImage( DWORD dwCarID, WORD wWidth, WORD wHeight, PBYTE pbPicData, DWORD dwImgDataLen, DWORD dwRecordType, DWORD64 dw64TimeMS )
{
    m_bResultComplete = false;
    if (NULL == m_CameraResult)
    {
        return -1;
    }
    char chlogInfo[260] = {0};
    sprintf(chlogInfo, "RecordInfoSmallImage  -begin- dwCarID = %ld", dwCarID);
    WriteLog(chlogInfo);
    int iBuffLen = 100 * 1024;
    if (m_CameraResult->dwCarID == dwCarID)
    {
        if (NULL != m_CameraResult->CIMG_PlateImage.pbImgData)
        {
            delete[] m_CameraResult->CIMG_PlateImage.pbImgData;
            m_CameraResult->CIMG_PlateImage.pbImgData = NULL;
        }
        m_CameraResult->CIMG_PlateImage.pbImgData = new BYTE[iBuffLen];
        if (m_CameraResult->CIMG_PlateImage.pbImgData != NULL)
        {
            memset(m_CameraResult->CIMG_PlateImage.pbImgData, 0 ,102400);
            HRESULT Hr = HVAPIUTILS_SmallImageToBitmapEx(pbPicData,
                                                         wWidth,
                                                         wHeight,
                                                         m_CameraResult->CIMG_PlateImage.pbImgData,
                                                         &iBuffLen);
            if ( Hr == S_OK)
            {
                m_CameraResult->CIMG_PlateImage.wImgWidth = wWidth;
                m_CameraResult->CIMG_PlateImage.wImgHeight = wHeight;
                m_CameraResult->CIMG_PlateImage.dwImgSize = iBuffLen;
            }
            else
            {
                WriteLog("HVAPIUTILS_SmallImageToBitmapEx 失败.");
            }
        }
    }
    char chlogInfo2[260] = {0};
    sprintf(chlogInfo2, "RecordInfoSmallImage  -end- dwCarID = %ld", dwCarID);
    WriteLog(chlogInfo2);
    return 0;
}

int Camera6467::RecordInfoBinaryImage( DWORD dwCarID, WORD wWidth, WORD wHeight, PBYTE pbPicData, DWORD dwImgDataLen, DWORD dwRecordType, DWORD64 dw64TimeMS )
{
    m_bResultComplete = false;

    if (NULL == m_CameraResult)
    {
        return -1;
    }
    char chlogInfo[260] = {0};
    sprintf(chlogInfo, "RecordInfoBinaryImage -begin- dwCarID = %ld", dwCarID);
    WriteLog(chlogInfo);

    if (m_CameraResult->dwCarID == dwCarID)
    {
        if (NULL != m_CameraResult->CIMG_BinImage.pbImgData)
        {
            delete[] m_CameraResult->CIMG_BinImage.pbImgData;
            m_CameraResult->CIMG_BinImage.pbImgData = NULL;
        }

        m_CameraResult->CIMG_BinImage.pbImgData = new BYTE[dwImgDataLen];
        m_CameraResult->CIMG_BinImage.dwImgSize = 0;
        if (m_CameraResult->CIMG_BinImage.pbImgData != NULL)
        {
            memcpy(m_CameraResult->CIMG_BinImage.pbImgData, pbPicData, dwImgDataLen);
            m_CameraResult->CIMG_BinImage.dwImgSize = dwImgDataLen;
            m_CameraResult->CIMG_BinImage.wImgHeight = wHeight;
            m_CameraResult->CIMG_BinImage.wImgWidth = wWidth;
        }
    }
    char chlogInfo2[260] = {0};
    sprintf(chlogInfo2, "RecordInfoBinaryImage -end- dwCarID = %ld", dwCarID);
    WriteLog(chlogInfo2);
    return 0;
}

void Camera6467::SetResultToUnready()
{
    m_bResultComplete = false;
}

void Camera6467::SetLocalListAndRemoteList(CustomResultList *pLocalList, CustomResultList *pRemoteList)
{
    if(pLocalList != NULL)
        g_pResultList_Local = pLocalList;
    if(pRemoteList != NULL)
        g_pResultList_Remote = pRemoteList;
}

void Camera6467::SaveResultToDisk(CameraResult* pResult)
{
    if(pResult == NULL)
        return;

    char chPath[260] = {0};
    sprintf(chPath, "%s/%s/%lld-%s", m_qstrResultPath.toStdString().c_str(), m_strIP.c_str(), pResult->dw64TimeMS, pResult->chPlateNO);
    QString qstrFilrDir = QString("%1").arg(chPath);
    QDir tempDir;
    int iPosition = qstrFilrDir.lastIndexOf("/");
    if(!tempDir.mkpath(qstrFilrDir.mid(0, iPosition)))
    {
        qDebug()<<"SaveResultToDisk  create dir failed.";
        return;
    }
    if(pResult->CIMG_FullImage.pbImgData)
    {
        long dwImgSize = pResult->CIMG_FullImage.dwImgSize;
        char chFilePath[260] = {0};
        sprintf(chFilePath, "%s.jpg", chPath);
        FILE* fp = fopen(chFilePath, "wb+");
        if(fp)
        {
            fwrite(pResult->CIMG_FullImage.pbImgData, 1 , dwImgSize, fp);
            fclose(fp);
            fp = NULL;
        }
    }
    if(pResult->CIMG_PlateImage.pbImgData)
    {
        long dwImgSize = pResult->CIMG_PlateImage.dwImgSize;
        char chFilePath[260] = {0};
        sprintf(chFilePath, "%s.bmp", chPath);
        FILE* fp = fopen(chFilePath, "wb+");
        if(fp)
        {
            fwrite(pResult->CIMG_PlateImage.pbImgData, 1 , dwImgSize, fp);
            fclose(fp);
            fp = NULL;
        }
    }
    if(pResult->CIMG_BinImage.pbImgData)
    {
        long dwImgSize = pResult->CIMG_BinImage.dwImgSize;
        char chFilePath[260] = {0};
        sprintf(chFilePath, "%s.bin", chPath);
        FILE* fp = fopen(chFilePath, "wb+");
        if(fp)
        {
            fwrite(pResult->CIMG_BinImage.pbImgData, 1 , dwImgSize, fp);
            fclose(fp);
            fp = NULL;
        }
    }
}

bool Camera6467::GetDeviceTime( DeviceTime& deviceTime )
{
    if (NULL == m_hHvHandle)
        return false;

    char chRetBuf[1024] = {0};
    int nRetLen = 0;

    if (HVAPI_ExecCmdEx(m_hHvHandle, "DateTime", chRetBuf, 1024, &nRetLen) != S_OK)
    {
        WriteLog("GetDeviceTime:: failed.");
        return false;
    }
    WriteLog(chRetBuf);
    bool bRet = false;
    const char* chFileName = "./DateTime.xml";
    //DeleteFileA(chFileName);
    QDir tempDir;
    tempDir.setPath(chFileName);
    tempDir.removeRecursively();

    FILE* file_L = fopen(chFileName, "wb");
    if(file_L)
    {
        size_t size_Read =  fwrite(chRetBuf, 1, nRetLen, file_L);
        fclose(file_L);
        file_L = NULL;
        char chFileLog[260] = {0};
        sprintf(chFileLog, "GetDeviceTime:: DateTime.xml create success, size =%d ", size_Read);
        WriteLog(chFileLog);
        bRet = true;
    }
    if (!bRet)
    {
        WriteLog("GetDeviceTime:: DateTime.xml create failed.");
        return false;
    }

    const char* pDate = NULL;
    const char* pTime = NULL;
    TiXmlDocument cXmlDoc;
    //    if(cXmlDoc.Parse(chRetBuf))
    if(cXmlDoc.LoadFile(chFileName))
    {
        TiXmlElement* pSectionElement = cXmlDoc.RootElement();
        if(pSectionElement)
        {
            TiXmlElement* pChileElement = pSectionElement->FirstChildElement();
            pDate = pChileElement->Attribute("Date");
            pTime = pChileElement->Attribute("Time");
        }
        else
        {
            WriteLog("find Root element failed.");
        }
    }
    else
    {
        WriteLog("parse failed");
    }
    int iYear = 0, iMonth = 0, iDay = 0, iHour = 0, iMinute = 0, iSecond = 0, iMiliSecond = 0;
    sscanf(pDate, "%04d.%02d.%02d", &iYear, &iMonth, &iDay);
    sscanf(pTime, "%02d:%02d:%02d %03d", &iHour, &iMinute, &iSecond, &iMiliSecond);

    deviceTime.iYear =iYear;
    deviceTime.iMonth =iMonth;
    deviceTime.iDay =iDay;
    deviceTime.iHour =iHour;
    deviceTime.iMinutes =iMinute;
    deviceTime.iSecond =iSecond;
    deviceTime.iMilisecond =iMiliSecond;

    return true;
}


