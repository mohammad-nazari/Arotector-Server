#pragma once
#ifndef _DBCONNECTION_H_
#define _DBCONNECTION_H_

#include "Server_SoapH.h"
#include "Header.h"
#include "Tools.h"
#include "../Split/Splitter.h"
#include "Protector.h"
#include "libmysqlwrapped.h"

class DBConnection
{
public:
	__declspec(dllexport)DBConnection(void);
	__declspec(dllexport)~DBConnection(void);

	__declspec(dllexport)int GetUsersList(std::vector<ns__User> &UserList);
	__declspec(dllexport)int AddUser(ns__User UserInfo);
	__declspec(dllexport)int UpdateUser(ns__User UserInfo);
	__declspec(dllexport)int DeleteUser(ns__User UserInfo);
	__declspec(dllexport)int AssignDeviceToUser(ns__UserDevice UserDeviceList);
	__declspec(dllexport)int DeleteDeviceFromUser(ns__UserDevice UserDeviceList);

	__declspec(dllexport)int UpdateDeviceRow(ns__Device deviceInfo);
	__declspec(dllexport)int UpdateDeviceRowSetting(ns__Device deviceInfo);
	__declspec(dllexport)int CreateDeviceRow(ns__Device deviceInfo);
	__declspec(dllexport)int CheckSerialNumber(std::string Input);
	__declspec(dllexport)int CheckUser(ns__User &UserInfo);
	__declspec(dllexport)int CheckDevice(ns__Device DeviceInfo);
	__declspec(dllexport)int GetDeviceSetting(ns__Device& DeviceInfo);
	__declspec(dllexport)int CheckUserDevice(ns__User UserInfo, ns__Device DeviceInfo, int AllowType = 0);
	__declspec(dllexport)int GetUserDeviceList(ns__User UserInfo, std::vector<ns__DeviceRules> &DeviceList);
	__declspec(dllexport)int GetAllDeviceList();
	__declspec(dllexport)int SaveDeviceStatus(ns__Device DeviceObject, std::string DeviceStatusString);

	__declspec(dllexport)std::vector<ns__DeviceRules> getUDeviceList();
	__declspec(dllexport)void setUserDeviceList(std::vector<ns__Device> UserDeviceList);

	__declspec(dllexport)int GetAllCityLoactionName(ns__AllCityLocatoins &AllCityLocationName);
	__declspec(dllexport)int GetCitiesName(std::vector<std::string> &AllCityName);
	__declspec(dllexport)int GetLacationsName(std::string CityName, std::vector<std::string> &AllLocationName);

	__declspec(dllexport)int ChangeCityName(std::string OldCityName, std::string NewCityName);
	__declspec(dllexport)int ChangeLocationName(std::string oldCityName, std::string newCityName, std::string OldLocationName, std::string NewLocationName);
	__declspec(dllexport)int ChangeDeviceName(int DeviceSerialNumber, std::string newCityName, std::string newLocationName, std::string NewDeviceName);
	__declspec(dllexport)int GetDeviceStatusLog(ns__DeviceStatusInfo StatusLogInfo, ns__DeviceStatusLogList &StatusLogList);

private:
	Tools toolsObject;
	Database *connection;
	StderrLog log;
	std::vector<ns__DeviceRules> userDeviceList;

	bool ConnectToDatabase();
	int GetDeviceInfo(int DeviceId, ns__DeviceRules& DeviceInformation);
	int GetUserInfo(ns__User& UserInformation);
};

#endif // _DBCONNECTION_H_
