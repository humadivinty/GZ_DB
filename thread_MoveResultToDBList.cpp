#include "thread_MoveResultToDBList.h"
#include "Camera6467.h"
#include <QDebug>
#include <QSettings>
#include <QDir>


Thread_MoveResultToDBList::Thread_MoveResultToDBList(void *pCamera, QObject *parent)
    :QThread(parent)
{
    m_pCamera = NULL;
    if(pCamera != NULL)
        m_pCamera = pCamera;
    m_bExit = false;
    InitParameter();
}

Thread_MoveResultToDBList::~Thread_MoveResultToDBList()
{
    m_pCamera = NULL;
    m_bExit = true;
}

void Thread_MoveResultToDBList::run()
{
    if(m_pCamera == NULL)
        return;
    Camera6467* pCamera = (Camera6467*)m_pCamera;
    while(true)
    {
        usleep(50);
        if(m_bExit)
        {
            qDebug()<<"end";
            break;
        }
        if(NULL == pCamera || NULL == pCamera->m_pResultList)
        {
            qDebug()<<"NULL == pCamera || NULL == pCamera->m_pResultList";
            usleep(50);
            continue;
        }
        CameraResult* ptempReult = NULL;
        ptempReult = pCamera->m_pResultList->GetOneResult();
        if(NULL == ptempReult)
        {
            //qDebug()<<"NULL == ptempReult";
            usleep(50);
            continue;
        }
        if(m_bSaveDB_Local && NULL != pCamera->g_pResultList_Local )
        {
            qDebug()<<"put one Result in local list";
            CameraResult* ptempReult_Local = new CameraResult(*ptempReult);
            int iRet = pCamera->g_pResultList_Local->PutOneResult(ptempReult_Local);
            if(iRet == 0)
            {
                //delete ptempReult_Local;
                ptempReult_Local = NULL;
            }
            else
            {
                //加入队列失败，缓存一份到本地缓存目录
                QString qstrLocalCacheFilePath = QString("%1/%2.ini").arg(m_qstrLocalCachePath_MoveThread).arg(ptempReult_Local->chListNo);
                SerializationAsConfigToDisk(qstrLocalCacheFilePath.toStdString().c_str(), *ptempReult_Local);

                delete ptempReult_Local;
                ptempReult_Local = NULL;
            }
        }
        if(m_bSaveDB_Remote && NULL != pCamera->g_pResultList_Remote )
        {
            qDebug()<<"put one Result in remote list";
            CameraResult* ptempReult_Remote = new CameraResult(*ptempReult);
            int iRet = pCamera->g_pResultList_Remote->PutOneResult(ptempReult_Remote);
            if(iRet == 0)
            {
                //delete ptempReult_Remote;
                ptempReult_Remote = NULL;
            }
            else
            {
                //加入队列失败，缓存一份到远程缓存目录
                QString qstrRemoteCacheFilePath = QString("%1/%2.ini").arg(m_qstrRemoteCachePath_MoveThread).arg(ptempReult_Remote->chListNo);
                SerializationAsConfigToDisk(qstrRemoteCacheFilePath.toStdString().c_str(), *ptempReult_Remote);

                delete ptempReult_Remote;
                ptempReult_Remote = NULL;
            }
        }
        if(NULL != ptempReult)
        {
            //qDebug()<<"delete the result.";
            delete ptempReult;
            ptempReult = NULL;
        }
    }
    qDebug()<<"out of Thread_MoveResultToDBList.";
}

void Thread_MoveResultToDBList::StopThread()
{
    m_bExit = true;
    m_pCamera = NULL;
}

void Thread_MoveResultToDBList::InitParameter()
{
    QString qstrCurrentPath = QDir::currentPath();
    qstrCurrentPath.append("/XLW_Config.ini");
    QSettings App_cfg(qstrCurrentPath,QSettings::IniFormat );

    //读取本地结果缓存信息
    m_bSaveDB_Local = App_cfg.value("LocalDataBase/Enable").toBool();
    m_qstrLocalCachePath_MoveThread = App_cfg.value("LocalDataBase/CachePath","./Local_CachePath/").toString();

    //读取远程结果缓存信息
    m_bSaveDB_Remote = App_cfg.value("RemoteDataBase/Enable").toBool();
    m_qstrRemoteCachePath_MoveThread = App_cfg.value("RemoteDataBase/CachePath", "./Remote_Cache/").toString();
}
