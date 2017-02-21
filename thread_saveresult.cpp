#include "thread_MoveResultToDBList.h"
#include "Camera6467.h"
#include <QDebug>


Thread_MoveResultToDBList::Thread_MoveResultToDBList(void *pCamera, QObject *parent)
    :QThread(parent)
{
    m_pCamera = NULL;
    if(pCamera != NULL)
        m_pCamera = pCamera;
    m_bExit = false;
}

Thread_MoveResultToDBList::~Thread_MoveResultToDBList()
{
    m_pCamera = NULL;
    m_bExit = true;
}

void Thread_MoveResultToDBList::run()
{
    qDebug()<<"begin";
    if(m_pCamera == NULL)
        return;
    Camera6467* pCamera = (Camera6467*)m_pCamera;
    qDebug()<<"begin2";
    while(true)
    {
        usleep(50);
        if(m_bExit)
        {
            qDebug()<<"end";
            break;
        }
        if(NULL == pCamera->m_pResultList)
        {
            usleep(50);
            continue;
        }
        CameraResult* ptempReult = NULL;
        ptempReult =pCamera->m_pResultList->GetOneResult();
        if(NULL == ptempReult)
            continue;
        if(m_bSaveDB_Local && NULL != pCamera->g_pResultList_Local )
        {
            qDebug()<<"put one Result in local list";
            CameraResult* ptempReult_Local = new CameraResult(*ptempReult);
            int iRet = pCamera->g_pResultList_Local->PutOneResult(ptempReult_Local);
            if(iRet == 0)
            {
                ptempReult_Local = NULL;
            }
            else
            {
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
                ptempReult_Remote = NULL;
            }
            else
            {
                delete ptempReult_Remote;
                ptempReult_Remote = NULL;
            }
        }

        delete ptempReult;
        ptempReult = NULL;
    }
    qDebug()<<"end 2";
}

void Thread_MoveResultToDBList::StopThread()
{
    m_bExit = true;
}
