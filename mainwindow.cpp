#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Camera6467.h"



Camera6467* g_Camera;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    for(int i = 0; i< MAX_CAMERA_COUNT; i++)
    {
        m_CameraGroup[i] = NULL;
        m_CameraInfoGroup[i] = NULL;
    }

    m_pThread_dbLocal = NULL;
    m_pThread_dbRemote = NULL;
    m_ConnectThread = NULL;
    m_pThread_Status = NULL;
    m_pThread_ReadLocalCache = NULL;
    m_pThread_ReadRemoteCache = NULL;

    m_DbContrl = NULL;
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    if(NULL != m_pThread_ReadLocalCache)
    {
        qDebug()<<"stop m_pThread_ReadLocalCache";
        m_pThread_ReadLocalCache->StopThread();
    }
    if(NULL != m_pThread_ReadRemoteCache)
    {
        qDebug()<<"stop m_pThread_ReadRemoteCache";
        m_pThread_ReadRemoteCache->StopThread();
    }

    if(NULL != m_pThread_dbLocal)
    {
        qDebug()<<"stop m_pThread_dbLocal";
        m_pThread_dbLocal->StopThread();
    }
    if(NULL != m_pThread_dbRemote)
    {
        qDebug()<<"stop m_pThread_dbRemote";
        m_pThread_dbRemote->StopThread();
    }
    if(NULL != m_pThread_Status)
    {
        qDebug()<<"stop m_pThread_Status";
        m_pThread_Status->StopThread();
    }
    //    if(NULL != m_ConnectThread)
    //    {
    //        delete m_ConnectThread;
    //        m_ConnectThread = NULL;
    //    }

    for(int i = 0; i< MAX_CAMERA_COUNT; i++)
    {
        if(NULL != m_CameraGroup[i])
        {
            qDebug()<<"delete CameraGroup "<< i;
            delete m_CameraGroup[i];
            m_CameraGroup[i] = NULL;
        }
        if(NULL  != m_CameraInfoGroup[i])
        {
            qDebug()<<"delete m_CameraInfoGroup "<< i;
            delete m_CameraInfoGroup[i];
            m_CameraInfoGroup[i] = NULL;
        }
    }

    qDebug()<<"m_LocalResultList.ClearAllResult()";
    m_LocalResultList.ClearAllResult();
    qDebug()<<"m_RemoteResultList.ClearAllResult()";
    m_RemoteResultList.ClearAllResult();

    if(NULL != m_DbContrl)
    {
        qDebug()<<"delete m_DbContrl ";
        delete m_DbContrl;
        m_DbContrl = NULL;
    }

    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    DBINFO tempDbInfo;
    tempDbInfo.iDBPort = 3263;  //这个可以通过查询sql server得来 , 执行语句 exec sys.sp_readerrorlog 0, 1, 'listening'
    tempDbInfo.qstrDataBaseName = QString("KKDB");
    tempDbInfo.qstrHostName = QString("172.18.11.54");
    tempDbInfo.qstrUserName = QString("sa");
    tempDbInfo.qstrPassWord = QString("Mypassword123456");
    m_DbContrl = new DBContrl(tempDbInfo);
    m_DbContrl->ConnetToDB();
}

void MainWindow::on_pushButton_2_clicked()
{
    char chFileName[260] = {0};
    sprintf(chFileName, "./QQ.jpg");
    long ifileSize = getFileSize(chFileName);
    qDebug()<<"file size = "<<ifileSize;

    unsigned char* ucByte = NULL;
    ucByte = new unsigned char[ifileSize];
    memset(ucByte, 1, ifileSize);
    qDebug()<<ucByte;

    FILE* file_temp = fopen(chFileName, "rb");
    if(file_temp)
    {
        size_t size_Read =  fread(ucByte, 1, ifileSize, file_temp);
        qDebug()<<"read size = "<<size_Read;
        fclose(file_temp);
        file_temp = NULL;
    }

    if(m_DbContrl)
    {
        m_DbContrl->InsertImageToDB(ucByte, ifileSize);
    }


    QByteArray tempQvariant((char*)ucByte,ifileSize);

    if(ucByte)
    {
        delete[] ucByte;
        ucByte = NULL;
    }

    unsigned char* uch2= NULL;

    uch2 = (unsigned char*)tempQvariant.data();

    FILE* fileWrite = fopen("./QQ23.jpg", "wb+");
    if(fileWrite)
    {
        long lWriteSize = fwrite(uch2, 1, tempQvariant.size(), fileWrite);
        qDebug()<<"Write size = "<<lWriteSize;
        fclose(fileWrite);
        fileWrite = NULL;
    }
    tempQvariant.clear();
    qDebug()<<"after clear ,size = "<<tempQvariant.size();
    if(tempQvariant.isNull())
    {
        qDebug()<<"after clear ,variant is NULL";
    }
    if(tempQvariant.isEmpty())
    {
        qDebug()<<"after clear ,variant is empty";
    }
    uch2 = NULL;
}

long MainWindow::getFileSize(const char *FileName)
{
    FILE* tmpFile = fopen(FileName, "rb");
    if(tmpFile)
    {
        fseek(tmpFile, 0, SEEK_END);
        long fileSize = ftell(tmpFile);
        fclose(tmpFile);
        tmpFile = NULL;
        qDebug()<<fileSize;
        return fileSize;
    }
    else
    {
        qDebug()<<"open file failed.";
        return 0;
    }

}

void MainWindow::on_pushButton_3_clicked()
{
    CameraInfo tempInfo;
    sprintf(tempInfo.chIP, "172.18.81.27");
    g_Camera = new Camera6467();
    g_Camera->SetCameraInfo(tempInfo);
    if(g_Camera->ConnectToCamera())
    {
        qDebug()<<"Connect to Camera success";
    }
    else
    {
        qDebug()<<"Connect failed";
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    //本地数据库插入线程
    m_pThread_dbLocal = new Thread_DB_Local("ThreadLocal", &m_LocalResultList, this);
    connect(m_pThread_dbLocal, &Thread_DB_Local::finished, m_pThread_dbLocal, &QObject::deleteLater);
    m_pThread_dbLocal->start();

    //远程数据库插入线程
    m_pThread_dbRemote = new Thread_DB_Remote("ThreadRemote", &m_RemoteResultList, this);
    connect(m_pThread_dbRemote, &Thread_DB_Remote::finished, m_pThread_dbRemote, &QObject::deleteLater);
    m_pThread_dbRemote->start();

    //设备状态插入线程
    m_pThread_Status = new Thread_DB_SaveStatus("ThreadStatus", m_CameraGroup, this);
    connect(m_pThread_Status, &Thread_DB_SaveStatus::finished, m_pThread_Status, &QObject::deleteLater);
    m_pThread_Status->start();

    //设备连接线程
    m_ConnectThread = new Thread_Connect_Camera(this);
    connect(m_ConnectThread, &Thread_Connect_Camera::finished, m_ConnectThread, &QObject::deleteLater);
    m_ConnectThread->SetCameraAndInfo(m_CameraGroup, m_CameraInfoGroup);
    m_ConnectThread->SetLocalAndRemoteList(&m_LocalResultList, &m_RemoteResultList);
    m_ConnectThread->start();

    //读取本地数据库结果缓存结果线程
    m_pThread_ReadLocalCache = new Thread_ReadCacheResult("Thread_ReadLocalCache", this);
    connect(m_pThread_ReadLocalCache, &Thread_ReadCacheResult::finished, m_pThread_ReadLocalCache, &QObject::deleteLater);
    if(NULL != m_pThread_dbLocal)
    {
        connect(m_pThread_dbLocal, &Thread_DB_Local::Signal_DBConnectingChange, m_pThread_ReadLocalCache, &Thread_ReadCacheResult::slot_ChangeUpLoadFlag);
    }
    m_pThread_ReadLocalCache->SetResutlListAndType(&m_LocalResultList, 1);
    m_pThread_ReadLocalCache->start();

    //读取远程数据库结果缓存结果线程
    m_pThread_ReadRemoteCache = new Thread_ReadCacheResult("Thread_ReadRemoteCache", this);
    connect(m_pThread_ReadRemoteCache, &Thread_ReadCacheResult::finished, m_pThread_ReadRemoteCache, &QObject::deleteLater);
    if(NULL != m_pThread_dbRemote)
    {
        connect(m_pThread_dbRemote, &Thread_DB_Remote::Signal_DBConnectingChange, m_pThread_ReadRemoteCache, &Thread_ReadCacheResult::slot_ChangeUpLoadFlag);
    }
    m_pThread_ReadRemoteCache->SetResutlListAndType(&m_RemoteResultList, 2);
    m_pThread_ReadRemoteCache->start();
}
