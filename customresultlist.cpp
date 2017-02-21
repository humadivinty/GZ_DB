#include "customresultlist.h"
#include <algorithm>
#include <QDebug>


CustomResultList::CustomResultList()
{
    m_bExit = false;
}

CustomResultList::~CustomResultList()
{
    m_bExit = true;
    m_KeywordBufferList.clear();
    //m_ResultList.clear();
    ClearAllResult();
    qDebug()<<"out of CustomResultList";
}

CameraResult *CustomResultList::GetOneResult()
{
    CameraResult* tempResult = NULL;
    m_qMutex.lock();
    if (m_ResultList.size() > 0)
    {
        //qDebug()<<"get one exist record.";
        tempResult = m_ResultList.front();
        m_ResultList.pop_front();
    }
    m_qMutex.unlock();
    return tempResult;
}

int CustomResultList::PutOneResult(CameraResult *pResult)
{
    int iRet = -1;
    if (NULL == pResult)
    {
        return iRet;
    }

    m_qMutex.lock();
    if (m_ResultList.size() >= MAX_RESULT_COUNT)
    {
        iRet = -2;
    }
    else
    {
        //CameraResult * pTempResult = new CameraResult(*pResult);
        m_ResultList.push_back(pResult);
        //pTempResult = NULL;
        iRet = 0;
    }
    m_qMutex.unlock();
    return iRet;

    //    m_qMutex.lock();
    //    if (m_ResultList.size() >= MAX_RESULT_COUNT)
    //    {
    //        iRet = -1;
    //    }
    //    else
    //    {
    //        if (strstr(pTempResult->chPlateNO, "нч"))
    //        {
    //            m_ResultList.push_back(pResult);
    //            iRet = 0;
    //        }
    //        else
    //        {
    //            std::string tempPlate(pTempResult->chPlateNO);
    //            if (HasKeyWord(tempPlate))
    //            {
    //                iRet = -2;
    //            }
    //            else
    //            {
    //                if(m_KeywordBufferList.size() > 5)
    //                {
    //                    m_KeywordBufferList.pop_front();
    //                }
    //                m_KeywordBufferList.push_back(tempPlate);
    //                m_ResultList.push_back(pTempResult);
    //                iRet = 0;
    //            }
    //        }
    //    }
    //    if(iRet == 0)
    //    {
    //        pTempResult = NULL;
    //    }
    //    else
    //    {
    //        delete pTempResult;
    //        pTempResult = NULL;
    //    }
    //    m_qMutex.unlock();
    //    return iRet;

}

void CustomResultList::ClearResult()
{
    m_ResultList.clear();
    m_KeywordBufferList.clear();
}

void CustomResultList::ClearAllResult()
{
    m_qMutex.lock();
    CameraResult* ptempReult = NULL;
    int iResultCount = m_ResultList.size();
    while ( iResultCount > 0)
    {
        ptempReult = m_ResultList.front();
        m_ResultList.pop_front();
        if(ptempReult != NULL)
        {
            delete ptempReult;
            ptempReult = NULL;
        }
        iResultCount = m_ResultList.size();
    }
    m_qMutex.unlock();
}

int CustomResultList::GetCount()
{
    int iSize = -1;
    m_qMutex.lock();
    iSize = m_ResultList.size();
    m_qMutex.unlock();
    return iSize;
}

bool CustomResultList::HasKeyWord(std::string strKeyWord)
{
    std::list<std::string>::iterator it1;
    it1 = find(m_KeywordBufferList.begin(), m_KeywordBufferList.end(), strKeyWord);
    if (it1 != m_KeywordBufferList.end())
    {
        return true;
    }
    return false;
}
