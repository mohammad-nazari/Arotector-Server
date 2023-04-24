#pragma once

#ifndef _PARSER_H_
#define _PARSER_H_

#include "SyntaxChecker.h"
#include "DBConnection.h"
#include "Tools.h"

class ExecuteRequest
{
public:
	ExecuteRequest(void);
	~ExecuteRequest(void);

	int CheckSyntax(std::string Input, int CommandType);
	int GetUsersList(std::vector<ns__User> &UserList);
	int AddUser(ns__User UserInfo);
	int UpdateUser(ns__User UserInfo);
	int DeleteUser(ns__User UserInfo);
	int AssignDeviceToUser(ns__UserDevice UserDeviceList);
	int DeleteDeviceFromUser(ns__UserDevice UserDeviceList);
	int GetUserDeviceList(ns__User UserInfo, std::vector<ns__DeviceRules> &DeviceListObject);
	int GetAllDeviceList(std::vector<ns__DeviceRules> &DeviceListObject);
	int AuthenticateUser(ns__User &UserInfo);
	int CheckDevice(ns__Device DeviceInfo);
	int GetDeviceSetting(ns__Device& DeviceInfo);
	int SetDeviceSetting(ns__Device DeviceInfo);
	int IsDeviceForUser(ns__User UserInfo, ns__Device DeviceInfo, int AllowType = 0);

	int DeviceInfoToBase64(ns__Device &DeviceObject);
	int DeviceInfoFromBase64(ns__Device &DeviceObject);

	int GetDeviceCityLocationList(ns__AllCityLocatoins &AllCityLocation);

	int ChangeCityName(std::string OldCityName, std::string NewCityName);
	int ChangeLocationName(std::string OldCityName, std::string NewCityName, std::string OldLocationName, std::string NewLocationName);
	int ChangeDeviceName(int DeviceSerialNumber, std::string NewCityName, std::string NewLocationName, std::string NewDeviceName);

	int GetDeviceStatusLog(ns__DeviceStatusInfo StatusLogInfo, ns__DeviceStatusLogList &StatusLogList);

protected:
	ns__Device deviceObjectResponse;
	ns__Device deviceObjectRequest;
	ns__ErrorCode errorObject;
	SyntaxChecker scObject;
	DBConnection dbObject;
	Base64 base64Object;
};
#endif

