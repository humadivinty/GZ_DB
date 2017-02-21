#ifndef DBCONTRL
#define DBCONTRL
#include<QtSql>
#include<QSqlError>
#include"CameraResult.h"
#include "Camera6467.h"

struct DBINFO
{
    QString qstrHostName;
    QString qstrDataBaseName;
    QString qstrUserName;
    QString qstrPassWord;
    QString qstrConnectName;
    int iDBPort;
    DBINFO()
    {
        iDBPort = 0;
    }
};

class DBContrl
{
public:
    DBContrl(DBINFO& dbInfo);
    ~DBContrl();

    bool ConnetToDB();
    bool CloseConnect();
    int InsertNormalData_Local(void* CamRecord);
    int InsertNormalData_Remote(void* CamRecord);
    bool InsertBigImg_Local(unsigned char* pImage, char* ListNo,long nImageLen);
    bool InsertSmallImg_Local(unsigned char* pImage, char* ListNo,long nImageLen);
    bool InsertSatus_Local(char* chListNo, int iDeviceID, char* chStatuMessage, char* chCreateTime, int iStatu);
    bool InsertSatus_Remote(char* chListNo, int iDeviceID, char* chStatuMessage, char* chCreateTime, int iStatu);

    bool InsertImageToDB(unsigned char* pImage, long imageSize);
    bool InitCameraInfo(CameraInfo** InfoGroup);

    bool IsConnect();
private:    

    QString m_qstrHostName;
    QString m_qstrDataBaseName;
    QString m_qstrUserName;
    QString m_qstrPassWord;
    int m_iDBPort;

    QMutex m_logMutex;
private:


protected:
    QSqlDatabase m_db;
    QString m_qstrConnectName;
     void WriteLog(const QString qstrLog);
};

#endif // DBCONTRL

