#include "ExecuteRequest.h"


ExecuteRequest::ExecuteRequest(void)
{
}


ExecuteRequest::~ExecuteRequest(void)
{
}


//************************************
// Method:    CheckSyntax
// FullName:  ExecuteRequest::CheckSyntax
// Access:    protected 
// Returns:   int
// Qualifier:
// Parameter: std::string Input
// Parameter: int CommandType
//
// Description: Get result from device and check syntax and revers it into Object
//************************************
int ExecuteRequest::CheckSyntax(std::string Input, int CommandType)
{
	switch(CommandType)
	{
		case STTCOMMAND:
		{
			return this->scObject.CheckSTTSyntax(Input);
		}
		default:
			return 0;
	}
}

int ExecuteRequest::AddUser(ns__User UserInfo)
{
	return this->dbObject.AddUser(UserInfo);
}

int ExecuteRequest::DeleteUser(ns__User UserInfo)
{
	return this->dbObject.DeleteUser(UserInfo);
}

//************************************
// Method:    AuthenticateUser
// FullName:  ExecuteRequest::AuthenticateUser
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: ns__User UserInfo
//
// Description: Generate a connection to database and check user information
//************************************
int ExecuteRequest::AuthenticateUser(ns__User &UserInfo)
{
	return this->dbObject.CheckUser(UserInfo);
}

int ExecuteRequest::AssignDeviceToUser(ns__UserDevice UserDeviceList)
{
	return this->dbObject.AssignDeviceToUser(UserDeviceList);
}

int ExecuteRequest::CheckDevice(ns__Device DeviceInfo)
{
	return this->dbObject.CheckDevice(DeviceInfo);
}

int ExecuteRequest::IsDeviceForUser(ns__User UserInfo, ns__Device DeviceInfo, int AllowType/*=0*/)
{
	return this->dbObject.CheckUserDevice(UserInfo, DeviceInfo, AllowType);
}

int ExecuteRequest::GetUserDeviceList(ns__User UserInfo, std::vector<ns__DeviceRules> &DeviceListObject)
{
	if(this->dbObject.GetUserDeviceList(UserInfo, DeviceListObject) > 0)
	{
	}
	else
	{
		// No device found
		return 0;
	}

	return 1;
}

int ExecuteRequest::GetDeviceSetting(ns__Device& DeviceInfo)
{
	return this->dbObject.GetDeviceSetting(DeviceInfo);
}

int ExecuteRequest::SetDeviceSetting(ns__Device DeviceInfo)
{
	return this->dbObject.UpdateDeviceRowSetting(DeviceInfo);
}

int ExecuteRequest::GetAllDeviceList(std::vector<ns__DeviceRules> &DeviceListObject)
{
	if(this->dbObject.GetAllDeviceList() > 0)
	{
		DeviceListObject = this->dbObject.getUDeviceList();
	}
	else
	{
		// No device found
		return 0;
	}

	return 1;
}

int ExecuteRequest::DeviceInfoToBase64(ns__Device &DeviceObject)
{
	DeviceObject.deviceNikeName = this->base64Object.Encode(DeviceObject.deviceNikeName);
	DeviceObject.deviceLocation = this->base64Object.Encode(DeviceObject.deviceLocation);
	DeviceObject.deviceCity = this->base64Object.Encode(DeviceObject.deviceCity);

	return 1;
}

int ExecuteRequest::DeviceInfoFromBase64(ns__Device &DeviceObject)
{
	DeviceObject.deviceNikeName = this->base64Object.Decode(DeviceObject.deviceNikeName);
	DeviceObject.deviceLocation = this->base64Object.Decode(DeviceObject.deviceLocation);
	DeviceObject.deviceCity = this->base64Object.Decode(DeviceObject.deviceCity);

	return 1;
}

int ExecuteRequest::GetDeviceCityLocationList(ns__AllCityLocatoins &AllCityLocation)
{
	return this->dbObject.GetAllCityLoactionName(AllCityLocation);
}

int ExecuteRequest::ChangeCityName(std::string OldCityName, std::string NewCityName)
{
	return this->dbObject.ChangeCityName(OldCityName, NewCityName);
}

int ExecuteRequest::ChangeLocationName(std::string OldCityName, std::string NewCityName, std::string OldLocationName, std::string NewLocationName)
{
	return this->dbObject.ChangeLocationName(OldCityName, NewCityName, OldLocationName, NewLocationName);
}

int ExecuteRequest::ChangeDeviceName(int DeviceSerialNumber, std::string NewCityName, std::string NewLocationName, std::string NewDeviceName)
{
	return this->dbObject.ChangeDeviceName(DeviceSerialNumber, NewCityName, NewLocationName, NewDeviceName);
}

int ExecuteRequest::GetUsersList(std::vector<ns__User> &UserList)
{
	return this->dbObject.GetUsersList(UserList);
}

int ExecuteRequest::DeleteDeviceFromUser(ns__UserDevice UserDeviceList)
{
	return this->dbObject.DeleteDeviceFromUser(UserDeviceList);
}

int ExecuteRequest::UpdateUser(ns__User UserInfo)
{
	return this->dbObject.UpdateUser(UserInfo);
}

int ExecuteRequest::GetDeviceStatusLog(ns__DeviceStatusInfo StatusLogInfo, ns__DeviceStatusLogList &StatusLogList)
{
	return this->dbObject.GetDeviceStatusLog(StatusLogInfo, StatusLogList);
}

