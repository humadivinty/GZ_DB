#ifndef CAMERRESULT
#define CAMERRESULT
#include<windef.h>
#define MAX_RESULT_COUNT 200

class CameraIMG
{
public:
	CameraIMG();
	CameraIMG(const CameraIMG& CaIMG);
	~CameraIMG();
	WORD wImgWidth;
	WORD wImgHeight;
	DWORD dwImgSize;
	WORD  wImgType;
	char chSavePath[256];
	PBYTE pbImgData;

	CameraIMG& operator = (const CameraIMG& CaIMG);
};

class CameraResult
{
public:

	CameraResult();
	CameraResult(const CameraResult& CaRESULT);
	~CameraResult();

	DWORD dwCarID;
	int iDeviceID;
	int iPlateColor;
	int iPlateTypeNo;
	DWORD64 dw64TimeMS;
	int iSpeed;
	int iResultNo;
	int iVehTypeNo;
	int iVehBodyColorNo;
	int iVehBodyDeepNo;
	int iVehLenth;
	int iAreaNo;
	int iRoadNo;
	int iLaneNo;
	int iDirection;


	char chDeviceIp[32];
	char chPlateNO[32];
	char chPlateColor[10];
	char chListNo[256];
	char chPlateTime[256];
	char chSignStationID[50];
	char chSignStationName[50];
	char pcAppendInfo[2048];

	char chDeviceID[3];
	char chLaneID[3];


	CameraIMG CIMG_SpecialImage;
	CameraIMG CIMG_FullImage;
	CameraIMG CIMG_PlateImage;
	CameraIMG CIMG_BinImage;

	CameraResult& operator = (const CameraResult& CaRESULT);

	friend bool SerializationResultToDisk(const char* chFilePath, const CameraResult& CaResult);
	friend bool SerializationFromDisk(const char* chFilePath, CameraResult& CaResult);

	friend bool SerializationAsConfigToDisk(const char* chFilePath, const CameraResult& CaResult);
	friend bool SerializationAsConfigFromDisk(const char* chFilePath, CameraResult& CaResult);
};

typedef struct _tagSafeModeInfo
{
	int iEableSafeMode;
	char chBeginTime[256];
	char chEndTime[256];
	int index;
	int DataInfo;
	_tagSafeModeInfo()
	{
		iEableSafeMode = 0;
		memset(chBeginTime, 0, sizeof(chBeginTime));
		memset(chEndTime, 0, sizeof(chEndTime));
		index = 0;
		DataInfo = 0;
	}
}_tagSafeModeInfo;

#endif
