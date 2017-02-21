#ifndef CUSTOMRESULTLIST
#define CUSTOMRESULTLIST
#include <list>
#include <string>
#include <QMutex>
#include "CameraResult.h"

class CustomResultList
{
public:
    CustomResultList();
    ~CustomResultList();

    CameraResult* GetOneResult();
    int PutOneResult(CameraResult* pResult);
    void ClearResult();
    void ClearAllResult();
    int GetCount();
private:
    std::list<CameraResult*> m_ResultList;
    std::list<std::string> m_KeywordBufferList;
    QMutex m_qMutex;
    bool m_bExit;

    //HANDLE m_ThreadHandle;
    //CRITICAL_SECTION m_csResult;


private:
    //static unsigned int WINAPI Thread_UpdateKeyword(LPVOID lpParame);			//用于更新缓存队列元素

    bool HasKeyWord(std::string strKeyWord);
};
#endif // CUSTOMRESULTLIST

