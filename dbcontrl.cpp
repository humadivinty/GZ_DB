#include "dbcontrl.h"
#include <QDateTime>


DBContrl::DBContrl(DBINFO &dbInfo)
{
    m_qstrDataBaseName = dbInfo.qstrDataBaseName;
    m_qstrHostName = dbInfo.qstrHostName;
    m_qstrUserName = dbInfo.qstrUserName;
    m_qstrPassWord = dbInfo.qstrPassWord;
    m_qstrConnectName = dbInfo.qstrConnectName;
    m_iDBPort = dbInfo.iDBPort;
}

DBContrl::~DBContrl()
{

}

bool DBContrl::ConnetToDB()
{
    QString qstrConnectString = QString("Driver={sql server};server=%1;database=%2;uid=%3;pwd=%4;")
            .arg(m_qstrHostName)
            .arg(m_qstrDataBaseName)
            .arg(m_qstrUserName)
            .arg(m_qstrPassWord);

    qDebug()<<"Connect String :"<<qstrConnectString;
    WriteLog(qstrConnectString);

    m_db = QSqlDatabase::addDatabase("QODBC", m_qstrConnectName);
    m_db.setDatabaseName(qstrConnectString);
    if(!m_db.open())
    {
        QSqlError qerror = m_db.lastError();
        QString errorInfo = qerror.text();
        m_db.close();
        qDebug()<<errorInfo;
        WriteLog(errorInfo);
        return false;
    }
    qDebug()<<"Connect to DataBase success!";
    WriteLog("Connect to DataBase success!");
    return true;
}

int DBContrl::InsertNormalData_Local(void *pCamRecord)
{
    if(NULL == pCamRecord)
    {
        //return false;
        return -1;
    }

    if(!m_db.isOpen())
    {
        qDebug()<<"The local data base is not connect, reconnect to it.";
        if(true != m_db.open())
        {
            qDebug()<<"The local data base is disconnect.";
            return -1;
        }
    }

    CameraResult* pRecord = (CameraResult*)pCamRecord;

    char chCmdText[1024] = {0};
    char chValues[1024] = {0};
    char chColumns[1024] = {0};
    sprintf(chColumns, "%s", "ListNo,DeviceID,LaneNo ,AreaNo ,RoadNo,Optime,DirectionNo,VehPlate,VehPlateManual,VehPlateSoft,PlateColorNo,VehSpeed,VehBodyColorNo	,VehBodyDeepNo,VehTypeNo ,PlateTypeNo	,UpLoad ");
    sprintf(chValues, "('%s', %d, %d, %d, %d, '%s', %d, '%s', '%s', '%s',  %d, %d, %d, %d, %d, %d, %s)",
            pRecord->chListNo,
            pRecord->iDeviceID,
            pRecord->iLaneNo,
            pRecord->iAreaNo,
            pRecord->iRoadNo,
            pRecord->chPlateTime,
            pRecord->iDirection,
            pRecord->chPlateNO,
            /*pRecord->chVehPlateManual*/ "manual",
            /*pRecord->chVehPlateSoft*/ "softPlate",
            pRecord->iPlateColor,
            pRecord->iSpeed,
            pRecord->iVehBodyColorNo,
            pRecord->iVehBodyDeepNo,
            pRecord->iVehTypeNo,
            pRecord->iPlateTypeNo,
            "1");
    sprintf(chCmdText, "INSERT INTO %s(%s)VALUES %s", "CaptureList", chColumns, chValues);
    //qDebug()<<chCmdText;


    QSqlQuery query(m_db);
    QString qstrStatement = QString("INSERT INTO  CaptureList (ListNo,DeviceID,LaneNo ,AreaNo ,RoadNo,Optime,DirectionNo,VehPlate,VehPlateManual,VehPlateSoft,PlateColorNo,VehSpeed,VehBodyColorNo	,VehBodyDeepNo,VehTypeNo ,PlateTypeNo	,UpLoad)");
    query.prepare(qstrStatement+"values ("
                                ":ListNo,"
                                ":DeviceID,"
                                ":LaneNo ,"
                                ":AreaNo ,"
                                ":RoadNo,"
                                ":Optime,"
                                ":DirectionNo,"
                                ":VehPlate,"
                                ":VehPlateManual,"
                                ":VehPlateSoft,"
                                ":PlateColorNo,"
                                ":VehSpeed,"
                                ":VehBodyColorNo ,"
                                ":VehBodyDeepNo,"
                                ":VehTypeNo ,"
                                ":PlateTypeNo ,"
                                ":UpLoad)");

    query.bindValue(":ListNo", pRecord->chListNo);
    query.bindValue(":DeviceID", pRecord->iDeviceID);
    query.bindValue(":LaneNo", pRecord->iLaneNo);
    query.bindValue(":AreaNo", pRecord->iAreaNo);
    query.bindValue(":RoadNo", pRecord->iRoadNo);
    query.bindValue(":Optime", pRecord->chPlateTime);
    query.bindValue(":DirectionNo", pRecord->iDirection);
    query.bindValue(":VehPlate", QString::fromLocal8Bit(pRecord->chPlateNO));
    query.bindValue(":VehPlateManual", "VehPlateManual");
    query.bindValue(":VehPlateSoft", "VehPlateSoft");
    query.bindValue(":PlateColorNo", pRecord->iPlateColor);
    query.bindValue(":VehSpeed", pRecord->iSpeed);
    query.bindValue(":VehBodyColorNo", 255);
    query.bindValue(":VehBodyDeepNo", 255);
    query.bindValue(":VehTypeNo", 1);
    query.bindValue(":PlateTypeNo", 1);
    query.bindValue(":UpLoad", "1");

    char chLog[260] = {0};
    bool bRet = false;
    int iRet = -1;
    if(query.exec())
    {
        bRet = true;
        iRet = 0;
        sprintf(chLog, "Insert to CaptureList success, listNo = %s, plateNo= ",  pRecord->chListNo, pRecord->chPlateNO);
        WriteLog(chLog);
    }
    else
    {
        bRet = false;
        WriteLog(chCmdText);
        sprintf(chLog, "Insert to CaptureList failed, listNo = %s",  pRecord->chListNo);
        WriteLog(chLog);
        QString errorLog = query.lastError().text();
        WriteLog(errorLog);
        qDebug()<<errorLog;
        if(errorLog.contains("不存在或访问被拒绝"))
        {
            iRet = -1;
        }
        else
        {
            //其他原因
            iRet = -2;
        }
    }
    //return bRet;
    return iRet;
}

int DBContrl::InsertNormalData_Remote(void *pCamRecord)
{
    if(NULL == pCamRecord)
    {
        return false;
    }
    //    if(!IsConnect())
    //    {
    //        ConnetToDB();
    //    }
    if(!m_db.isOpen())
    {
        qDebug()<<"The data base is not connect, reconnect to it.";
        if(true != m_db.open())
        {
            qDebug()<<"The data base is disconnect.";
            return -1;
        }
    }

    CameraResult* pRecord = (CameraResult*)pCamRecord;

    char chCmdText[1024] = {0};
    char chValues[1024] = {0};
    char chColumns[1024] = {0};

    char szTimeFlag[256]={0};
    QDateTime dateTime = QDateTime::currentDateTime();
    sprintf(szTimeFlag, "%d.%02d.%02d %02d:%02d:%02d.%03d",
            dateTime.date().year(),
            dateTime.date().month(),
            dateTime.date().day(),
            dateTime.time().hour(),
            dateTime.time().minute(),
            dateTime.time().second(),
            dateTime.time().msec());
    QString qstrTimeFlag = QDateTime::currentDateTime().toString("yyyy.MM.dd HH:mm:ss.zzz");

    sprintf(chColumns, "%s", "ListNo, DeviceID, LaneNo, Optime, DirectionNo, VehPlate, VehPlateManual, VehPlateSoft, PlateColorNo, VehSpeed, VehBodyColorNo, VehBodyDeepNo, VehTypeNo, PlateTypeNo, TimeFlag");
    sprintf(chValues, "('%s',%d,%d,'%s',%d,'%s','%s','%s',%d,%d,%d,%d,%d,%d,'%s')",
            pRecord->chListNo,
            pRecord->iDeviceID,
            pRecord->iLaneNo,
            pRecord->chPlateTime,
            pRecord->iDirection,
            pRecord->chPlateNO,
            /* pRecord->chVehPlateManual*/ "Manu" ,
            /*pRecord->chVehPlateSoft*/ "Soft",
            pRecord->iPlateColor,
            pRecord->iSpeed,
            pRecord->iVehBodyColorNo,
            pRecord->iVehBodyDeepNo,
            pRecord->iVehTypeNo,
            pRecord->iPlateTypeNo,
            szTimeFlag);
    sprintf(chCmdText, "INSERT INTO %s(%s)VALUES %s", "HDVEHICLELIST", chColumns, chValues);

    QSqlQuery query(m_db);
    QString qstrStatement = QString("INSERT INTO HDVEHICLELIST (ListNo, DeviceID, LaneNo, Optime, DirectionNo, VehPlate, VehPlateManual, VehPlateSoft, PlateColorNo, VehSpeed, VehBodyColorNo, VehBodyDeepNo, VehTypeNo, PlateTypeNo, TimeFlag)");

    query.prepare(qstrStatement+"values ("
                                ":ListNo, "
                                ":DeviceID,"
                                ":LaneNo,"
                                ":Optime, "
                                ":DirectionNo,"
                                ":VehPlate, "
                                ":VehPlateManual, "
                                ":VehPlateSoft, "
                                ":PlateColorNo, "
                                ":VehSpeed, "
                                ":VehBodyColorNo, "
                                ":VehBodyDeepNo, "
                                ":VehTypeNo,"
                                ":PlateTypeNo,"
                                ":TimeFlag)");

    query.bindValue(":ListNo", pRecord->chListNo);
    query.bindValue(":DeviceID", pRecord->iDeviceID);
    query.bindValue(":LaneNo", pRecord->iLaneNo);
    query.bindValue(":Optime", pRecord->chPlateTime);
    query.bindValue(":DirectionNo", pRecord->iDirection);
    query.bindValue(":VehPlate", QString::fromLocal8Bit(pRecord->chPlateNO));
    query.bindValue(":VehPlateManual", "Manu");
    query.bindValue(":VehPlateSoft", "Soft");
    query.bindValue(":PlateColorNo", pRecord->iPlateColor);
    query.bindValue(":VehSpeed", pRecord->iSpeed);
    query.bindValue(":VehBodyColorNo",pRecord->iVehBodyColorNo);
    query.bindValue(":VehBodyDeepNo",pRecord->iVehBodyDeepNo);
    query.bindValue(":VehTypeNo", pRecord->iVehTypeNo);
    query.bindValue(":PlateTypeNo", pRecord->iPlateTypeNo);
    query.bindValue(":TimeFlag", qstrTimeFlag);

    char chLog[260] = {0};
    bool bRet = false;
    bool iRet = -1;
    if(query.exec())
    {
        bRet = true;
        iRet = 0;
        sprintf(chLog, "Insert to HDVEHICLELIST success, listNo = %s",  pRecord->chListNo);
        WriteLog(chLog);
    }
    else
    {
        bRet = false;
        WriteLog(chCmdText);
        sprintf(chLog, "Insert to HDVEHICLELIST failed, listNo = %s",  pRecord->chListNo);
        WriteLog(chLog);
        QString errorLog = query.lastError().text();
        WriteLog(errorLog);
        qDebug()<<errorLog;
        qDebug()<<query.lastQuery();

        if(errorLog.contains("不存在或访问被拒绝"))
        {
            iRet = -1;
        }
        else
        {
            //其他原因
            iRet = -2;
        }
    }
    //return bRet;
    return iRet;
}

bool DBContrl::InsertBigImg_Local(unsigned char *pImage, char *ListNo, long imageSize)
{
    if(NULL ==  pImage || 0 == imageSize)
    {
        QString qstrlog = QString("ListNo = %1 , image is empty ,CarFullView insert failed.").arg(ListNo);
        WriteLog(qstrlog);
        return false;
    }

    if(!m_db.isOpen())
    {
        qDebug()<<"InsertBigImg_Local::The local data base is not connect, reconnect to it.";
        if(true != m_db.open())
        {
            qDebug()<<"InsertBigImg_Local:: The local data base is disconnect.";
            return false;
        }
    }

    QSqlQuery tempQuery(m_db);
    tempQuery.prepare("insert into CarFullView([ListNo] ,[FullViewPhoto] ,[OpTime] ,[UpLoad])" "values(:ListNo, :FullViewPhoto,:OpTime, :UpLoad )");
    QByteArray tempArray((char*)pImage, imageSize);
    tempQuery.bindValue(":ListNo", ListNo);
    tempQuery.bindValue(":FullViewPhoto",tempArray);
    QString tempDateTime = QDateTime::currentDateTime().toString("yyyy.MM.dd HH:mm:ss.zzz");
    qDebug()<<tempDateTime;
    tempQuery.bindValue(":OpTime", tempDateTime);
    tempQuery.bindValue(":UpLoad", "1");

    char chLog[260] = {0};
    bool bRet = false;
    if(tempQuery.exec())
    {
        bRet = true;
        sprintf(chLog, "Insert to CarFullView success, listNo = %s",  ListNo);
        WriteLog(chLog);
        qDebug()<<chLog;
    }
    else
    {
        bRet = false;
        sprintf(chLog, "Insert to CarFullView failed, listNo = %s",  ListNo);
        WriteLog(chLog);
        QString errorLog = tempQuery.lastError().text();
        WriteLog(errorLog);
        qDebug()<<errorLog;
    }
    tempArray.clear();

    return bRet;
}

bool DBContrl::InsertSmallImg_Local(unsigned char *pImage, char *ListNo, long nImageLen)
{
    if(NULL ==  pImage || 0 == nImageLen)
    {
        QString qstrlog = QString("ListNo = %1 , image is empty ,CarCloseUp insert failed.").arg(ListNo);
        WriteLog(qstrlog);
        return false;
    }
    if(!m_db.isOpen())
    {
        qDebug()<<"InsertSmallImg_Local::The local data base is not connect, reconnect to it.";
        if(true != m_db.open())
        {
            qDebug()<<"InsertSmallImg_Local:: The local data base is disconnect.";
            return false;
        }
    }

    QSqlQuery tempQuery(m_db);
    tempQuery.prepare("insert into CarCloseUp([ListNo] ,[CloseUpPhoto] ,[OpTime] ,[UpLoad])"  "values(:ListNo, :CloseUpPhoto, :OpTime, :UpLoad )");
    QByteArray tempArray((char*)pImage, nImageLen);
    tempQuery.bindValue(":ListNo", ListNo);
    tempQuery.bindValue(":CloseUpPhoto",tempArray);
    QString tempDateTime = QDateTime::currentDateTime().toString("yyyy.MM.dd HH:mm:ss.zzz");
    qDebug()<<tempDateTime;
    tempQuery.bindValue(":OpTime", tempDateTime);
    tempQuery.bindValue(":UpLoad", "1");

    char chLog[260] = {0};
    bool bRet = false;
    if(tempQuery.exec())
    {
        bRet = true;
        sprintf(chLog, "Insert to CarCloseUp success, listNo = %s",  ListNo);
        WriteLog(chLog);
        qDebug()<<chLog;
    }
    else
    {
        bRet = false;
        sprintf(chLog, "Insert to CarCloseUp failed, listNo = %s",  ListNo);
        WriteLog(chLog);
        QString errorLog = tempQuery.lastError().text();
        WriteLog(errorLog);
        qDebug()<<errorLog;
    }
    tempArray.clear();

    return bRet;
}

bool DBContrl::InsertSatus_Local(char *chListNo, int iDeviceID, char *chStatuMessage, char *chCreateTime, int iStatu)
{
    if(!m_db.isOpen())
    {
        qDebug()<<"InsertSatus_Local::The local data base is not connect, reconnect to it.";
        if(true != m_db.open())
        {
            qDebug()<<"InsertSatus_Local:: The local data base is disconnect.";
            return false;
        }
    }

    char chCmdTextp[1024] = {0};
    char chValues[1024] ={0};
    char chClounName[256]={0};
    char chUpload[1] ={0};

    //构造并填充命令
    sprintf(chClounName,"%s","ListNo,DeviceStatusNo,DeviceID,ExceptionMessage,Optime,UpLoad");
    sprintf(chValues, "('%s', %d, %d, '%s', '%s', '%s')", chListNo, iStatu, iDeviceID, chStatuMessage, chCreateTime, chUpload);
    sprintf(chCmdTextp, "INSERT INTO %s(%s) VALUES %s", "BayonetStatus",chClounName, chValues);

    QSqlQuery tempQuery(m_db);
    tempQuery.prepare("INSERT INTO BayonetStatus (ListNo,DeviceStatusNo,DeviceID,ExceptionMessage,Optime,UpLoad) VALUES (:ListNo, :DeviceStatusNo, :DeviceID, :ExceptionMessage, :Optime, :UpLoad)");
    tempQuery.bindValue(":ListNo", chListNo);
    tempQuery.bindValue(":DeviceStatusNo", iStatu);
    tempQuery.bindValue(":DeviceID", iDeviceID);
    tempQuery.bindValue(":ExceptionMessage", chStatuMessage);
    tempQuery.bindValue(":Optime", /*QDateTime::currentDateTime().toString("yyyy.MM.dd HH:mm:ss.zzz")*/ chCreateTime);
    tempQuery.bindValue(":UpLoad", "1");

    char chLog[260] = {0};
    bool bRet = false;
    if(tempQuery.exec())
    {
        bRet = true;
        sprintf(chLog, "Insert to BayonetStatus success, listNo = %s", chListNo);
        WriteLog(chLog);
    }
    else
    {
        bRet = false;
        sprintf(chLog, "Insert to BayonetStatus failed, listNo = %s",  chListNo);
        WriteLog(chLog);
        QString errorLog = tempQuery.lastError().text();
        WriteLog(errorLog);
        qDebug()<<errorLog;
    }
    return bRet;
}

bool DBContrl::InsertSatus_Remote(char *chListNo, int iDeviceID, char *chStatuMessage, char *chCreateTime, int iStatu)
{
    if(!m_db.isOpen())
    {
        qDebug()<<"InsertSatus_Remote::The local data base is not connect, reconnect to it.";
        if(true != m_db.open())
        {
            qDebug()<<"InsertSatus_Remote:: The local data base is disconnect.";
            return false;
        }
    }

    char chCmdTextp[1024] = {0};
    char chValues[1024] ={0};
    char chClounName[256]={0};
    char chUpload[1] ={0};

    //构造并填充命令
    sprintf(chClounName,"%s","ListNo,DeviceID,DEVICESTATUSNO,ExceptionMessage,Optime,TimeFlag");
    sprintf(chValues, "('%s', %d, %d, '%s', '%s', '%s')", chListNo, iStatu, iDeviceID, chStatuMessage, chCreateTime, chUpload);
    sprintf(chCmdTextp, "INSERT INTO %s(%s) VALUES %s", "HDCardListState",chClounName, chValues);

    QSqlQuery tempQuery(m_db);
    tempQuery.prepare("INSERT INTO HDCardListState (ListNo, DeviceID, DEVICESTATUSNO, ExceptionMessage, Optime, TimeFlag) VALUES (:ListNo, :DeviceID, :DEVICESTATUSNO, :ExceptionMessage, :Optime, :TimeFlag)");
    tempQuery.bindValue(":ListNo", chListNo);
    tempQuery.bindValue(":DeviceID", iDeviceID);
    tempQuery.bindValue(":DEVICESTATUSNO", iStatu);
    tempQuery.bindValue(":ExceptionMessage", chStatuMessage);
    tempQuery.bindValue(":Optime", chCreateTime);
    tempQuery.bindValue(":TimeFlag", chCreateTime);

    char chLog[260] = {0};
    bool bRet = false;
    if(tempQuery.exec())
    {
        bRet = true;
        sprintf(chLog, "Insert to HDCardListState success, listNo = %s",  chListNo);
        WriteLog(chLog);
    }
    else
    {
        bRet = false;
        sprintf(chLog, "Insert to HDCardListState failed, listNo = %s",  chListNo);
        WriteLog(chLog);
        QString errorLog = tempQuery.lastError().text();
        WriteLog(errorLog);
        qDebug()<<errorLog;
    }
    return bRet;
}

bool DBContrl::InitCameraInfo(CameraInfo** InfoGroup)
{
    QSqlQuery tempQuery(m_db);
    if(tempQuery.exec("SELECT *  FROM [Hve_Addr]"))
    {
        int iCount = 0;
        while(tempQuery.next())
        {
            QString strIPAddr= tempQuery.value(0).toString();
            int iDeviceID = tempQuery.value(1).toInt();
            int iLaneNo = tempQuery.value(2).toInt();
            int iAreaNo = tempQuery.value(3).toInt();
            int iRoadNo = tempQuery.value(4).toInt();
            int iDirectionNo = tempQuery.value(5).toInt();
            int iRoadCount = tempQuery.value(6).toInt();
            QString strKKInfo1 = tempQuery.value(7).toString();
            QString strKKInfo2 = tempQuery.value(8).toString();

            //            if(NULL == InfoGroup[iCount])
            //            {
            //                InfoGroup[iCount] = new CameraInfo();
            //            }

            //           InfoGroup[InfoGroup]->iAreaNo = iAreaNo;
            //           InfoGroup[InfoGroup]->iDirection = iDirectionNo;
            //           InfoGroup[InfoGroup]->iRoadNo = iRoadNo;
            //            sprintf(InfoGroup[InfoGroup]->chIP, "%s", strIPAddr.toStdString().c_str());
            //            sprintf(InfoGroup[InfoGroup]->chKKInfo1, "%s", strKKInfo1.toStdString().c_str());
            //            sprintf(InfoGroup[InfoGroup]->chKKInfo2, "%s", strKKInfo2.toStdString().c_str());
            if(NULL == *InfoGroup)
                (*InfoGroup) = new CameraInfo();

            (*InfoGroup)->iAreaNo = iAreaNo;
            (*InfoGroup)->iDirection = iDirectionNo;
            (*InfoGroup)->iRoadNo = iRoadNo;
            sprintf((*InfoGroup)->chIP, "%s", strIPAddr.toStdString().c_str());
            sprintf((*InfoGroup)->chKKInfo1, "%s", strKKInfo1.toStdString().c_str());
            sprintf((*InfoGroup)->chKKInfo2, "%s", strKKInfo2.toStdString().c_str());

            (*InfoGroup)->bIsInit = true;

            qDebug()<<strIPAddr<<strKKInfo1<<strKKInfo2<<iDeviceID<<iLaneNo<<iRoadCount;

            InfoGroup++;
            iCount++;
            if(iCount > MAX_CAMERA_COUNT)
            {
                WriteLog("CameraCount is full ,stop to add");
                break;
            }
        }
        qDebug()<<"finish get camera info";
        return true;
    }
    else
    {
        printf("%s", tempQuery.lastError().text().toStdString().c_str());
        qDebug()<<tempQuery.lastError().text();
    }
    return false;
}

bool DBContrl::IsConnect()
{
    return m_db.isOpen();
}

bool DBContrl::InsertImageToDB(unsigned char *pImage, long imageSize)
{
    QSqlQuery tempQuery(m_db);
    tempQuery.prepare("insert into CarCloseUp([ListNo] ,[CloseUpPhoto] ,[OpTime] ,[UpLoad])" "values(:ListNo, :CloseUpPhoto,:OpTime, :UpLoad )");
    QByteArray tempArray((char*)pImage, imageSize);
    QVariant tempVariante = QVariant::fromValue((void*)pImage);
    tempQuery.bindValue(":ListNo", QString("%1").arg(QDateTime::currentMSecsSinceEpoch()));
    //tempQuery.bindValue(":CloseUpPhoto",tempArray);
    tempQuery.bindValue(":CloseUpPhoto",tempVariante);
    QString tempDateTime = QDateTime::currentDateTime().toString("yyyy.MM.dd HH:mm:ss.zzz");
    qDebug()<<tempDateTime;
    tempQuery.bindValue(":OpTime", tempDateTime);
    tempQuery.bindValue(":UpLoad", 1);

    if(tempQuery.exec())
    {
        qDebug()<<"insert success";
        //tempArray.clear();
        tempVariante.clear();
        return true;
    }
    else
    {
        QSqlError sqlError= tempQuery.lastError();
        QString ErrorInfo = sqlError.text();
        qDebug()<<ErrorInfo;
        //tempArray.clear();
        tempVariante.clear();
        return false;
    }
}

void DBContrl::WriteLog(const QString qstrLogBuff)
{
    if(qstrLogBuff.isEmpty())
        return;
    QDir tempDir;
    QString qstrCurrentPath = QDir::currentPath();
    qstrCurrentPath.append("//SNW_log//");
    if(!tempDir.exists(qstrCurrentPath))
    {
        tempDir.mkdir(qstrCurrentPath);
    }
    QDate currentDate = QDate::currentDate();
    QTime currentTime = QTime::currentTime();
    QString qstrFileName = QString(qstrCurrentPath+"//DB_log_%1_%2.log").arg(m_qstrConnectName).arg(currentDate.toString("yyyy-MM-dd"));

    m_logMutex.lock();
    QFile logFile(qstrFileName);
    if(!logFile.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug()<<"open log File failed";
        return;
    }
    QString qstrLog = QString("%1 %2: %3").arg(currentDate.toString("yyyy-MM-dd")).arg(currentTime.toString("hh:mm:ss zzz")).arg(qstrLogBuff);

    QTextStream qStreamLog(&logFile);
    qStreamLog<<qstrLog<<endl;

    logFile.close();
    m_logMutex.unlock();
}

