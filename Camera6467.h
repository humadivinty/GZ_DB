#ifndef CAMERA6467_H
#define CAMERA6467_H
#include "CameraResult.h"
#include "customresultlist.h"
#include "thread_MoveResultToDBList.h"
#include<windef.h>
#include <QMutex>
#include <string>
#include <list>

#define MAX_CAMERA_COUNT 50

struct CameraInfo
{
	char chIP[20];                          //deviceIP
	char chDeviceID[3];
	char chStationID[7];
	char chLaneID[3];
    char chKKInfo1[260];
    char chKKInfo2[260];
	bool bLogEnable;
	bool bSynTimeEnable;
	bool bIsInit;
	int iDirection;
    int iAreaNo;
    int iRoadNo;
    int iDeviceID;
	CameraInfo()
	{
		memset(chIP, 0, 20);
		memset(chDeviceID, 0, 3);
		memset(chStationID, 0, 7);
		memset(chLaneID, 0, 3);
        memset(chKKInfo1, 0, 260);
        memset(chKKInfo2, 0, 260);
		bLogEnable = true;
		bSynTimeEnable = false;
		bIsInit = false;
		iDirection = 0;
        iAreaNo = 0;
        iRoadNo = 0;
        iDeviceID = 0;
	}
};

struct SaveModeInfo
{
	int iSafeModeEnable;
	char chBeginTime[260];
	char chEndTime[260];
	int iIndex;
	int iDataType;
	SaveModeInfo()
	{
		iSafeModeEnable = 0;
		iIndex = 0;
		iDataType = 0;
		memset(chBeginTime, 0, 260);
		memset(chEndTime, 0, 260);
	}
};

struct DeviceTime
{
	int iYear;
	int iMonth;
	int iDay;
	int iHour;
	int iMinutes;
	int iSecond;
	int iMilisecond;
	
	DeviceTime()
	{
		iYear = 0;
		iMonth = 0;
		iDay = 0;
		iHour = 0;
		iMinutes = 0;
		iSecond = 0;
		iMilisecond = 0;
	}
};

class Camera6467
{
public:
	Camera6467();
	Camera6467(const char* chIP, HWND*  hWnd,int Msg);
	~Camera6467();

	//void SetList(Result_lister* ResultList);
	bool SetCameraInfo(CameraInfo& camInfo);

	int GetCamStatus();
    int GetDeviceID();
	char* GetStationID();
    //char* GetDeviceID();
	char*  GetLaneID();
	const char* GetCameraIP();
	CameraResult* GetOneResult();
	bool GetDeviceTime(DeviceTime& deviceTime);

	bool ConnectToCamera();
	void ReadConfig();
	bool WriteLog(const char* chlog);
	bool TakeCapture();
	bool SynTime();
	bool SynTime(int Year, int Month, int Day, int Hour, int Minute, int Second, int MilientSecond);
	bool SaveImgToDisk( char* chImgPath, BYTE* pImgData, DWORD dwImgSize);
    //bool SaveImgToDisk( char* chImgPath, BYTE* pImgData, DWORD dwImgSize, int iWidth, int iHeight, int iType = 0);
    void SetResultToUnready();
    void SetLocalListAndRemoteList(CustomResultList* pLocalList, CustomResultList* pRemoteList);

private:
	void* m_hHvHandle;
	void* m_hWnd;
	int m_iMsg;
	std::string m_strIP;
	std::string m_strDeviceID;
	int m_iConnectStatus;
    unsigned long m_dwConnectStatus;
    int m_iCompressQuality;
    int m_iDirection;
    int m_iDeviceID;


	char m_chDeviceID[3];
	char m_chStationID[7];
	char m_chLaneID[3];
    char m_chBackupPath[260];
    char m_chtrRootPath[260];

	bool m_bLogEnable;
	bool m_bSynTime;
	bool m_bResultComplete;
    bool m_bSaveDB_Local;
    bool m_bSaveDB_Remote;
    bool m_bExit;

    QString m_qstrResultPath;
    QString m_qstrBackupPath;

    QString m_qstrLocalCachePath;
    QString m_qstrRemoteCachePath;

	SaveModeInfo m_SaveModelInfo;

	CameraResult* m_CameraResult;
	CameraResult* m_BufferResult;

    Thread_MoveResultToDBList* m_pThreadMoveResult;

    QMutex m_qMutex_Log;
    QMutex m_qMutex_Result;

    //int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
    //bool GetStreamLength( IStream* pStream, ULARGE_INTEGER* puliLenth );
    //bool SaveImgToDiskByQt( CameraIMG& camImageStruct);
    //void SetImgPath(CameraResult* camResult);
    //CRITICAL_SECTION m_csLog;
    //CRITICAL_SECTION m_csResult;
    void ReadHistoryInfo();
    void WriteHistoryInfo(SaveModeInfo& SaveInfo);
    void AnalysisAppendInfo(CameraResult* CamResult);
    void SaveResultToDisk(CameraResult* pResult);

public:
    CustomResultList*  m_pResultList;
    CustomResultList*  g_pResultList_Local;
    CustomResultList*  g_pResultList_Remote;

public:
	static int  RecordInfoBeginCallBack(PVOID pUserData, DWORD dwCarID)
	{
		if ( pUserData == NULL )
			return 0;

		Camera6467* pThis = (Camera6467*)pUserData;
		return pThis->RecordInfoBegin(dwCarID);
	};
	int RecordInfoBegin(DWORD dwCarID);


	static int  RecordInfoEndCallBack(PVOID pUserData, DWORD dwCarID)
	{
		if ( pUserData == NULL )
			return 0;

		Camera6467* pThis = (Camera6467*)pUserData;
		return pThis->RecordInfoEnd(dwCarID);
	};
	int RecordInfoEnd(DWORD dwCarID);


	static int  RecordInfoPlateCallBack(PVOID pUserData,
		DWORD dwCarID,
		LPCSTR pcPlateNo,
		LPCSTR pcAppendInfo,
		DWORD dwRecordType,
		DWORD64 dw64TimeMS)
	{
		if (pUserData == NULL)
			return 0;

		Camera6467* pThis = (Camera6467*)pUserData;
		return pThis->RecordInfoPlate(dwCarID, pcPlateNo, pcAppendInfo, dwRecordType, dw64TimeMS);
	}
	int RecordInfoPlate(DWORD dwCarID,
		LPCSTR pcPlateNo,
		LPCSTR pcAppendInfo,
		DWORD dwRecordType,
		DWORD64 dw64TimeMS);


	static int  RecordInfoBigImageCallBack(PVOID pUserData,
		DWORD dwCarID,
		WORD  wImgType,
		WORD  wWidth,
		WORD  wHeight,
		PBYTE pbPicData,
		DWORD dwImgDataLen,
		DWORD dwRecordType,
		DWORD64 dw64TimeMS)
	{
		if (pUserData == NULL)
			return 0;

		Camera6467* pThis = (Camera6467*)pUserData;
		return pThis->RecordInfoBigImage(dwCarID, wImgType, wWidth, wHeight, pbPicData, dwImgDataLen, dwRecordType, dw64TimeMS);
	}
	int RecordInfoBigImage(DWORD dwCarID,
		WORD  wImgType,
		WORD  wWidth,
		WORD  wHeight,
		PBYTE pbPicData,
		DWORD dwImgDataLen,
		DWORD dwRecordType,
		DWORD64 dw64TimeMS);



	static int  RecordInfoSmallImageCallBack(PVOID pUserData,
		DWORD dwCarID,
		WORD wWidth,
		WORD wHeight,
		PBYTE pbPicData,
		DWORD dwImgDataLen,
		DWORD dwRecordType,
		DWORD64 dw64TimeMS)
	{
		if (pUserData == NULL)
			return 0;

		Camera6467* pThis = (Camera6467*)pUserData;
		return pThis->RecordInfoSmallImage(dwCarID, wWidth, wHeight, pbPicData, dwImgDataLen, dwRecordType, dw64TimeMS);
	}
	int RecordInfoSmallImage(DWORD dwCarID,
		WORD wWidth,
		WORD wHeight,
		PBYTE pbPicData,
		DWORD dwImgDataLen,
		DWORD dwRecordType,
		DWORD64 dw64TimeMS);


	static int  RecordInfoBinaryImageCallBack(PVOID pUserData,
		DWORD dwCarID,
		WORD wWidth,
		WORD wHeight,
		PBYTE pbPicData,
		DWORD dwImgDataLen,
		DWORD dwRecordType,
		DWORD64 dw64TimeMS)
	{
		if (pUserData == NULL)
			return 0;

		Camera6467* pThis = (Camera6467*)pUserData;
		return pThis->RecordInfoBinaryImage(dwCarID, wWidth, wHeight, pbPicData, dwImgDataLen, dwRecordType, dw64TimeMS);
	}
	int RecordInfoBinaryImage(DWORD dwCarID,
		WORD wWidth,
		WORD wHeight,
		PBYTE pbPicData,
		DWORD dwImgDataLen,
		DWORD dwRecordType,
		DWORD64 dw64TimeMS);

};

#endif
