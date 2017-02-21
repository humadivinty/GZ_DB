#include "CameraResult.h"
#include <QString>
#include <QDir>
#include <QSettings>

CameraIMG& CameraIMG::operator=( const CameraIMG& CaIMG )
{
	if (this != &CaIMG)
	{
		wImgWidth = CaIMG.wImgWidth;
		wImgHeight = CaIMG.wImgHeight;
		dwImgSize = CaIMG.dwImgSize;
		wImgType = CaIMG.wImgType;
		for (unsigned int i = 0; i< sizeof(chSavePath); i++)
		{
			chSavePath[i] = CaIMG.chSavePath[i];
		}
		if (NULL != CaIMG.pbImgData)
		{
			pbImgData = new BYTE[CaIMG.dwImgSize];
			for (DWORD j = 0; j< dwImgSize; j++)
			{
				pbImgData[j] = CaIMG.pbImgData[j];
			}
		}
		else
		{
			pbImgData = NULL;
		}
	}
	return *this;
}

CameraIMG::CameraIMG()
{
	wImgWidth = 0;
	wImgHeight = 0;
	pbImgData = NULL;
	dwImgSize = 0;
	for (unsigned int i = 0; i< sizeof(chSavePath); i++)
	{
		chSavePath[i] = 0;
	}
}

CameraIMG::CameraIMG( const CameraIMG& CaIMG )
{
	wImgWidth = CaIMG.wImgWidth;
	wImgHeight = CaIMG.wImgHeight;
	dwImgSize = CaIMG.dwImgSize;
	wImgType = CaIMG.wImgType;
	for (unsigned int i = 0; i< sizeof(chSavePath); i++)
	{
		chSavePath[i] = CaIMG.chSavePath[i];
	}
	if (NULL != CaIMG.pbImgData)
	{
		pbImgData = new BYTE[CaIMG.dwImgSize];
		for (DWORD j = 0; j< dwImgSize; j++)
		{
			pbImgData[j] = CaIMG.pbImgData[j];
		}
	}
	else
	{
		pbImgData = NULL;
	}
}

CameraIMG::~CameraIMG()
{
	wImgWidth = 0;
	wImgHeight = 0;
	if (NULL != pbImgData)
	{
		delete[] pbImgData;
		pbImgData = NULL;
	}
	dwImgSize = 0;
	memset(chSavePath, 0, sizeof(chSavePath));
}
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

CameraResult::CameraResult()
{
	dwCarID = 0;
	dw64TimeMS =0;
	iLaneNo = 0;
	iPlateColor = 0;
	iVehTypeNo = 0;
	iDirection = 0;
	iAreaNo = 0;
	iVehBodyColorNo = 0;
	iPlateTypeNo = 0;
	iVehBodyDeepNo = 0;
	iDeviceID = 0;
	iRoadNo = 0;
	iSpeed = 0;
	iVehLenth = 0;
	iResultNo =0;

	//pcAppendInfo = NULL;
	for (unsigned int i = 0; i < sizeof(pcAppendInfo); i++)
	{
		pcAppendInfo[i] = 0;
	}

	for (unsigned int i = 0; i < sizeof(chDeviceIp); i++)
	{
		chDeviceIp[i] = 0;
	}

	for (unsigned int i = 0; i < sizeof(chPlateNO); i++)
	{
		chPlateNO[i] = 0;
	}
	for (unsigned int i = 0; i < sizeof(chPlateColor); i++)
	{
		chPlateColor[i] = 0;
	}

	for (unsigned int i = 0; i < sizeof(chListNo); i++)
	{
		chListNo[i] = 0;
	}

	for (unsigned int i = 0; i < sizeof(chPlateTime); i++)
	{
		chPlateTime[i] = 0;
	}

	for (unsigned int i = 0; i < sizeof(chSignStationID); i++)
	{
		chSignStationID[i] = 0;
	}

	for (unsigned int i = 0; i < sizeof(chSignStationName); i++)
	{
		chSignStationName[i] = 0;
	}

	for (unsigned int i = 0; i < sizeof(chLaneID); i++)
	{
		chLaneID[i] = 0;
	}

	for (unsigned int i = 0; i < sizeof(chDeviceID); i++)
	{
		chDeviceID[i] = 0;
	}
}

CameraResult::CameraResult( const CameraResult& CaRESULT )
{
	dwCarID = CaRESULT.dwCarID;
	dw64TimeMS = CaRESULT.dw64TimeMS;
	iLaneNo = CaRESULT.iLaneNo;
	iPlateColor = CaRESULT.iPlateColor;
	iVehTypeNo = CaRESULT.iVehTypeNo;
	iDirection = CaRESULT.iDirection;
	iAreaNo = CaRESULT.iAreaNo;
	iVehBodyColorNo = CaRESULT.iVehBodyColorNo;
	iPlateTypeNo = CaRESULT.iPlateTypeNo;
	iVehBodyDeepNo = CaRESULT.iVehBodyDeepNo;
	iDeviceID = CaRESULT.iDeviceID;
	iRoadNo = CaRESULT.iRoadNo;
	iSpeed = CaRESULT.iSpeed;
	iVehLenth = CaRESULT.iVehLenth;
	iResultNo = CaRESULT.iResultNo;

	//pcAppendInfo = NULL;
	for (unsigned int i = 0; i < sizeof(pcAppendInfo); i++)
	{
		pcAppendInfo[i] = CaRESULT.pcAppendInfo[i];
	}

	for (unsigned int i = 0; i < sizeof(chDeviceIp); i++)
	{
		chDeviceIp[i] = CaRESULT.chDeviceIp[i];
	}

	for (unsigned int i = 0; i < sizeof(chPlateNO); i++)
	{
		chPlateNO[i] = CaRESULT.chPlateNO[i];
	}

	for (unsigned int i = 0; i < sizeof(chPlateColor); i++)
	{
		chPlateColor[i] = CaRESULT.chPlateColor[i];
	}

	for (unsigned int i = 0; i < sizeof(chListNo); i++)
	{
		chListNo[i] = CaRESULT.chListNo[i];
	}

	for (unsigned int i = 0; i < sizeof(chPlateTime); i++)
	{
		chPlateTime[i] = CaRESULT.chPlateTime[i];
	}

	for (unsigned int i = 0; i < sizeof(chSignStationID); i++)
	{
		chSignStationID[i] = CaRESULT.chSignStationID[i];
	}

	for (unsigned int i = 0; i < sizeof(chSignStationName); i++)
	{
		chSignStationName[i] = CaRESULT.chSignStationName[i];
	}

	for (unsigned int i = 0; i < sizeof(chLaneID); i++)
	{
		chLaneID[i] = CaRESULT.chLaneID[i];
	}

	for (unsigned int i = 0; i < sizeof(chDeviceID); i++)
	{
		chDeviceID[i] = CaRESULT.chDeviceID[i];
	}

	CIMG_SpecialImage = CaRESULT.CIMG_SpecialImage;
	CIMG_FullImage = CaRESULT.CIMG_FullImage;
	CIMG_PlateImage = CaRESULT.CIMG_PlateImage;
	CIMG_BinImage = CaRESULT.CIMG_BinImage;
}

CameraResult::~CameraResult()
{
	dwCarID =0;
	dw64TimeMS =0;
	iLaneNo = 0;
	iVehTypeNo = 0;
	iPlateColor = -1;
	iDirection = 0;
	iVehBodyColorNo = 0;
	iPlateTypeNo = 0;
	iVehBodyDeepNo = 0;
	iDeviceID = 0;
	iRoadNo = 0;
	iSpeed = 0;
	iVehLenth = 0;
	iResultNo =0;

	memset(chDeviceIp, 0, sizeof(chDeviceIp));
	memset(chPlateNO, 0, sizeof(chPlateNO));
	memset(chPlateColor, 0, sizeof(chPlateColor));
	memset(chListNo, 0, sizeof(chListNo));
	memset(chPlateTime, 0, sizeof(chPlateTime));
	memset(chSignStationID, 0, sizeof(chSignStationID));
	memset(chSignStationName, 0, sizeof(chSignStationName));
	memset(pcAppendInfo, 0, sizeof(pcAppendInfo));
	memset(chLaneID, 0, sizeof(chLaneID));
	memset(chDeviceID, 0, sizeof(chDeviceID));
}

CameraResult& CameraResult::operator=( const CameraResult& CaRESULT )
{
	dwCarID = CaRESULT.dwCarID;
	dw64TimeMS = CaRESULT.dw64TimeMS;
	iLaneNo = CaRESULT.iLaneNo;
	iPlateColor = CaRESULT.iPlateColor;
	iVehTypeNo = CaRESULT.iVehTypeNo;
	iDirection = CaRESULT.iDirection;
	iAreaNo = CaRESULT.iAreaNo;
	iVehBodyColorNo = CaRESULT.iVehBodyColorNo;
	iPlateTypeNo = CaRESULT.iPlateTypeNo;
	iVehBodyDeepNo = CaRESULT.iVehBodyDeepNo;
	iDeviceID = CaRESULT.iDeviceID;
	iRoadNo = CaRESULT.iRoadNo;
	iSpeed = CaRESULT.iSpeed;
	iVehLenth = CaRESULT.iVehLenth;
	iResultNo = CaRESULT.iResultNo;

	//pcAppendInfo = NULL;

	for(unsigned int i =0; i<sizeof(pcAppendInfo); i++)
	{
		pcAppendInfo[i] = 0;
	}

	for (unsigned int i = 0; i < sizeof(chDeviceIp); i++)
	{
		chDeviceIp[i] = CaRESULT.chDeviceIp[i];
	}

	for (unsigned int i = 0; i < sizeof(chPlateNO); i++)
	{
		chPlateNO[i] = CaRESULT.chPlateNO[i];
	}

	for (unsigned int i = 0; i < sizeof(chPlateColor); i++)
	{
		chPlateColor[i] = CaRESULT.chPlateColor[i];
	}

	for (unsigned int i = 0; i < sizeof(chListNo); i++)
	{
		chListNo[i] = CaRESULT.chListNo[i];
	}

	for (unsigned int i = 0; i< sizeof(chPlateTime); i++ )
	{
		chPlateTime[i] = CaRESULT.chPlateTime[i];
	}

	for (unsigned int i = 0; i < sizeof(chSignStationID); i++)
	{
		chSignStationID[i] = CaRESULT.chSignStationID[i];
	}

	for (unsigned int i = 0; i < sizeof(chSignStationName); i++)
	{
		chSignStationName[i] = CaRESULT.chSignStationName[i];
	}

	for (unsigned int i = 0; i < sizeof(chLaneID); i++)
	{
		chLaneID[i] = CaRESULT.chLaneID[i];
	}

	for (unsigned int i = 0; i < sizeof(chDeviceID); i++)
	{
		chDeviceID[i] = CaRESULT.chDeviceID[i];
	}

	CIMG_SpecialImage = CaRESULT.CIMG_SpecialImage;
	CIMG_FullImage = CaRESULT.CIMG_FullImage;
	CIMG_PlateImage = CaRESULT.CIMG_PlateImage;
	CIMG_BinImage = CaRESULT.CIMG_BinImage;

    return *this;
}

bool SerializationAsConfigToDisk(const char *chFilePath, const CameraResult &CaResult)
{
    if(NULL == chFilePath)
        return false;

    QSettings App_cfg(chFilePath,QSettings::IniFormat );

    App_cfg.setValue("iDeviceID", CaResult.iDeviceID);
    App_cfg.setValue("iPlateColor", CaResult.iPlateColor);
    App_cfg.setValue("iPlateTypeNo", CaResult.iPlateTypeNo);
    App_cfg.setValue("dw64TimeMS", CaResult.dw64TimeMS);
    App_cfg.setValue("iSpeed", CaResult.iSpeed);
    App_cfg.setValue("iResultNo", CaResult.iResultNo);
    App_cfg.setValue("iVehTypeNo", CaResult.iVehTypeNo);
    App_cfg.setValue("iVehBodyColorNo", CaResult.iVehBodyColorNo);
    App_cfg.setValue("iVehBodyDeepNo", CaResult.iVehBodyDeepNo);
    App_cfg.setValue("iVehLenth", CaResult.iVehLenth);
    App_cfg.setValue("iAreaNo", CaResult.iAreaNo);
    App_cfg.setValue("iRoadNo", CaResult.iRoadNo);
    App_cfg.setValue("iLaneNo", CaResult.iLaneNo);
    App_cfg.setValue("iDirection", CaResult.iDirection);

    App_cfg.setValue("chDeviceIp", QString(CaResult.chDeviceIp));
    App_cfg.setValue("chPlateNO", QString::fromLocal8Bit(CaResult.chPlateNO));
    App_cfg.setValue("chListNo", QString(CaResult.chListNo));
    App_cfg.setValue("chPlateTime", QString(CaResult.chPlateTime));
    App_cfg.setValue("chSignStationID", QString(CaResult.chSignStationID));
    App_cfg.setValue("chSignStationName", QString(CaResult.chSignStationName));
    App_cfg.setValue("pcAppendInfo", QString(CaResult.pcAppendInfo));
    App_cfg.setValue("chDeviceID", QString(CaResult.chDeviceID));
    App_cfg.setValue("chLaneID", QString(CaResult.chLaneID));

    //CIMG_SpecialImage
    App_cfg.setValue("CIMG_SpecialImage/wImgWidth", CaResult.CIMG_SpecialImage.wImgWidth);
    App_cfg.setValue("CIMG_SpecialImage/wImgHeight",CaResult.CIMG_SpecialImage.wImgHeight);
    App_cfg.setValue("CIMG_SpecialImage/dwImgSize", WORD(CaResult.CIMG_SpecialImage.dwImgSize));
    App_cfg.setValue("CIMG_SpecialImage/wImgType", CaResult.CIMG_SpecialImage.wImgType);
    App_cfg.setValue("CIMG_SpecialImage/chSavePath", QString::fromLocal8Bit(CaResult.CIMG_SpecialImage.chSavePath));
    //CIMG_FullImage
    App_cfg.setValue("CIMG_FullImage/wImgWidth", CaResult.CIMG_FullImage.wImgWidth);
    App_cfg.setValue("CIMG_FullImage/wImgHeight", CaResult.CIMG_FullImage.wImgHeight);
    App_cfg.setValue("CIMG_FullImage/dwImgSize", WORD(CaResult.CIMG_FullImage.dwImgSize));
    App_cfg.setValue("CIMG_FullImage/wImgType", CaResult.CIMG_FullImage.wImgType);
    App_cfg.setValue("CIMG_FullImage/chSavePath", QString::fromLocal8Bit(CaResult.CIMG_FullImage.chSavePath));
    //CIMG_PlateImage
    App_cfg.setValue("CIMG_PlateImage/wImgWidth", CaResult.CIMG_PlateImage.wImgWidth);
    App_cfg.setValue("CIMG_PlateImage/wImgHeight", CaResult.CIMG_PlateImage.wImgHeight);
    App_cfg.setValue("CIMG_PlateImage/dwImgSize", WORD(CaResult.CIMG_PlateImage.dwImgSize));
    App_cfg.setValue("CIMG_PlateImage/wImgType", CaResult.CIMG_PlateImage.wImgType);
    App_cfg.setValue("CIMG_PlateImage/chSavePath", QString::fromLocal8Bit(CaResult.CIMG_PlateImage.chSavePath));
    //CIMG_BinImage
    App_cfg.setValue("CIMG_BinImage/wImgWidth", CaResult.CIMG_BinImage.wImgWidth);
    App_cfg.setValue("CIMG_BinImage/wImgHeight", CaResult.CIMG_BinImage.wImgHeight);
    App_cfg.setValue("CIMG_BinImage/dwImgSize", WORD(CaResult.CIMG_BinImage.dwImgSize));
    App_cfg.setValue("CIMG_BinImage/wImgType", CaResult.CIMG_BinImage.wImgType);
    App_cfg.setValue("CIMG_BinImage/chSavePath", QString::fromLocal8Bit(CaResult.CIMG_BinImage.chSavePath));

    return true;
}

bool SerializationAsConfigFromDisk(const char *chFilePath, CameraResult &CaResult)
{
    if(NULL == chFilePath)
        return false;

    QSettings App_cfg(chFilePath,QSettings::IniFormat );

    CaResult.dwCarID = App_cfg.value("dwCarID").toInt();
    CaResult.iDeviceID = App_cfg.value("iDeviceID").toInt();
    CaResult.iPlateColor = App_cfg.value("iPlateColor").toInt();
    CaResult.iPlateTypeNo = App_cfg.value("iPlateTypeNo").toInt();
    CaResult.dw64TimeMS = App_cfg.value("dw64TimeMS").toInt();
    CaResult.iSpeed = App_cfg.value("iSpeed").toInt();
    CaResult.iResultNo = App_cfg.value("iResultNo").toInt();
    CaResult.iVehTypeNo = App_cfg.value("iVehTypeNo").toInt();
    CaResult.iVehBodyColorNo = App_cfg.value("iVehBodyColorNo").toInt();
    CaResult.iVehBodyDeepNo = App_cfg.value("iVehBodyDeepNo").toInt();
    CaResult.iVehLenth = App_cfg.value("iVehLenth").toInt();
    CaResult.iAreaNo = App_cfg.value("iAreaNo").toInt();
    CaResult.iRoadNo = App_cfg.value("iRoadNo").toInt();
    CaResult.iLaneNo = App_cfg.value("iLaneNo").toInt();
    CaResult.iDirection = App_cfg.value("iDirection").toInt();

    QString qstrTemp = QString("");
    qstrTemp = App_cfg.value("chDeviceIp").toString();
    sprintf(CaResult.chDeviceIp, "%s", qstrTemp.toStdString().data());

    qstrTemp = App_cfg.value("chPlateNO").toString();
    sprintf(CaResult.chPlateNO, "%s", qstrTemp.toLocal8Bit().data());

    qstrTemp = App_cfg.value("chListNo").toString();
    sprintf(CaResult.chListNo, "%s", qstrTemp.toStdString().c_str());

    qstrTemp = App_cfg.value("chPlateTime").toString();
    sprintf(CaResult.chPlateTime, "%s", qstrTemp.toStdString().c_str());

    qstrTemp = App_cfg.value("chSignStationID").toString();
    sprintf(CaResult.chSignStationID, "%s", qstrTemp.toStdString().c_str());

    qstrTemp = App_cfg.value("chSignStationName").toString();
    sprintf(CaResult.chSignStationName, "%s", qstrTemp.toStdString().c_str());

    qstrTemp = App_cfg.value("pcAppendInfo").toString();
    sprintf(CaResult.pcAppendInfo, "%s", qstrTemp.toLocal8Bit().data());

    qstrTemp = App_cfg.value("chDeviceID").toString();
    sprintf(CaResult.chDeviceID, "%s", qstrTemp.toStdString().c_str());

    qstrTemp = App_cfg.value("chLaneID").toString();
    sprintf(CaResult.chLaneID, "%s", qstrTemp.toStdString().c_str());

    CaResult.CIMG_SpecialImage.wImgWidth = App_cfg.value("CIMG_SpecialImage/wImgWidth").toInt();
    CaResult.CIMG_SpecialImage.wImgHeight = App_cfg.value("CIMG_SpecialImage/wImgHeight").toInt();
    CaResult.CIMG_SpecialImage.dwImgSize = App_cfg.value("CIMG_SpecialImage/dwImgSize").toInt();
    CaResult.CIMG_SpecialImage.wImgType = App_cfg.value("CIMG_SpecialImage/wImgType").toInt();
    qstrTemp = App_cfg.value("CIMG_SpecialImage/chSavePath").toString();
    sprintf(CaResult.CIMG_SpecialImage.chSavePath, "%s", qstrTemp.toStdString().c_str());

    CaResult.CIMG_FullImage.wImgWidth = App_cfg.value("CIMG_FullImage/wImgWidth").toInt();
    CaResult.CIMG_FullImage.wImgHeight = App_cfg.value("CIMG_FullImage/wImgHeight").toInt();
    CaResult.CIMG_FullImage.dwImgSize = App_cfg.value("CIMG_FullImage/dwImgSize").toInt();
    CaResult.CIMG_FullImage.wImgType = App_cfg.value("CIMG_FullImage/wImgType").toInt();
    qstrTemp = App_cfg.value("CIMG_FullImage/chSavePath").toString();
    sprintf(CaResult.CIMG_FullImage.chSavePath, "%s", qstrTemp.toStdString().c_str());

    CaResult.CIMG_PlateImage.wImgWidth = App_cfg.value("CIMG_PlateImage/wImgWidth").toInt();
    CaResult.CIMG_PlateImage.wImgHeight = App_cfg.value("CIMG_PlateImage/wImgHeight").toInt();
    CaResult.CIMG_PlateImage.dwImgSize = App_cfg.value("CIMG_PlateImage/dwImgSize").toInt();
    CaResult.CIMG_PlateImage.wImgType = App_cfg.value("CIMG_PlateImage/wImgType").toInt();
    qstrTemp = App_cfg.value("CIMG_PlateImage/chSavePath").toString();
    sprintf(CaResult.CIMG_PlateImage.chSavePath, "%s", qstrTemp.toStdString().c_str());

    CaResult.CIMG_BinImage.wImgWidth = App_cfg.value("CIMG_BinImage/wImgWidth").toInt();
    CaResult.CIMG_BinImage.wImgHeight = App_cfg.value("CIMG_BinImage/wImgHeight").toInt();
    CaResult.CIMG_BinImage.dwImgSize = App_cfg.value("CIMG_BinImage/dwImgSize").toInt();
    CaResult.CIMG_BinImage.wImgType = App_cfg.value("CIMG_BinImage/wImgType").toInt();
    qstrTemp = App_cfg.value("CIMG_BinImage/chSavePath").toString();
    sprintf(CaResult.CIMG_BinImage.chSavePath, "%s", qstrTemp.toStdString().c_str());

    return true;
}
