#pragma once

#ifndef _SCONNECTION_H_
#define _SCONNECTION_H_

#include "Server_SoapH.h"

#include "../Security/Base64.h"
#include "Header.h"
#include "Tools.h"
#include "SyntaxChecker.h"
#include "DBConnection.h"
#include <map>

class SConnection
{
public:
	__declspec(dllexport)SConnection();
	__declspec(dllexport)~SConnection(void);

	__declspec(dllexport)int CreateSocket();
	__declspec(dllexport)int CloseSocket();
	__declspec(dllexport)int StartListen();
	__declspec(dllexport)int HandleDevice(ns__Device Input);
	__declspec(dllexport)int ThreadStartListen();
	__declspec(dllexport)int ThreadHandleDevice(ns__Device DeviceObject);
	__declspec(dllexport)int GetDeviceStatus(ns__Device& DeviceObject);
	__declspec(dllexport)int SetDeviceStatus(ns__Device& DeviceObject);

private:
	SOCKET ListenSocket;
	int IsDevice(SOCKET pParam);
	int ReadData(SOCKET Socket, std::string& strMessage);
	int ReadData1(SOCKET Socket, std::string & strMessage);
	int WriteData(SOCKET Socket, const std::string strMessage);
	int AddUpdateDeletSearchDeviceInList(ns__Device& NewDevice, int CommandType, DeviceMap& NewDeviceMap);
	int UpdateDeviceRow(ns__Device deviceInfo);
	int CreateDeviceRow(ns__Device deviceInfo);
	int ApplayCommand(ns__Device DeviceObject, std::string& CommandString);
	std::map<std::string, std::string> GetSensorsName(std::string Input);
	void InitiateDeviceStatus(ns__Device DeviceObject, std::string &ErrorsString);
	int SaveDevieStatus(ns__Device DeviceObject, std::string DeviceStatusString);
	Tools toolsObject;
	CRITICAL_SECTION csDeviceList;
};
#endif // _SCONNECTION_H_
