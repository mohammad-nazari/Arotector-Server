//Content of file "protector.h": 
//gsoap ns service name: Protector
//gsoap ns service namespace: Protector
//gsoap ns schema namespace: Protector
//gsoap ns service location: http://localhost:9090

#import "stlvector.h"
#import "struct_tm.h"

enum ns__SettingLevel
{
	Normal,
	Disable,
	Warning,
	Minor,
	Major,
	Critical
};

enum ns__UserType
{
	Admin,
	Control,
	Monitor
};

enum ns__SimcardType
{
	MCI,
	IranCell,
	Taliya,
	RighTel,
	UnknownSim
};

enum ns__SensorType
{
	Multi,
	Mono
};

enum ns__SensorName
{
	TEMPERATURE,
	HUMIDITY,
	H2S,
	PRESSER,
	GAS,
	ACVOLTAGE,
	ACVOLTAGE1,
	ACVOLTAGE2,
	ACVOLTAGE3,
	ACAMPERE,
	ACAMPERE1,
	ACAMPERE2,
	ACAMPERE3,
	DCVOLTAGE,
	DCAMPERE,
	SMOKE,
	MAGNET,
	WATER,
	MOTION,
	DIGITAL,
	UNKNOWN
};

typedef std::string xsd__string;
typedef int xsd__int;
typedef bool xsd__boolean;
typedef double xsd__double;
typedef time_t  xsd__dateTime;
typedef long long xsd__long;

// Result of requests
class ns__ErrorCode
{
	xsd__int errorNumber = 0;
	xsd__string errorMessage = "";
	enum ns__SettingLevel errorType = Normal;
};

class ns__User
{
	xsd__int userId = 0;
	xsd__string userName = "";
	xsd__string userPassword = "";
	xsd__string userNewPassword = "";
	xsd__string userFirstName = "";	// For new password
	xsd__string userLastName = "";
	enum ns__UserType userType = Monitor; // Admin, User, Monitor
	xsd__string userKey = ""; // Key for encrypt decrypt
	xsd__dateTime userSubmitDateTime;	// Submitted date
	ns__ErrorCode userError;
};

class ns__UsersList
{
	std::vector<ns__User> usersListUsers;
	ns__ErrorCode usersListError;
};

class ns__IP
{
	xsd__int ipFirst = 0;
	xsd__int ipSecond = 0;
	xsd__int ipThird = 0;
	xsd__int ipFourth = 0;
};

class ns__Server
{
	xsd__string serverDNSAddress;	//DNS
	ns__IP serverIP;
	xsd__int serverPort = 0;
	ns__ErrorCode serverError;
};

class ns__Gprs
{
	xsd__string gprsDNSAddress;	//DNS
	ns__IP gprsIP;
	xsd__int gprsPort = 0;
	ns__ErrorCode gprsError;
};

class ns__Mobile
{
	xsd__long mobileNumber = 0;
	enum ns__SimcardType mobileSimCardType = MCI;
	xsd__int mobileChargeValue = 0;
	xsd__int mobileSignalValue = 0;
};

class ns__Relay
{
	xsd__int relayIndex = 0;
	xsd__boolean relayOnOff = false;
};

class ns__Sensor
{
	enum ns__SensorName sensorName = TEMPERATURE;
	xsd__string sensorNikeName = "";
	xsd__double sensorValue = 0.0;
	xsd__int sensorCalibration = 0;
	xsd__int sensorMinimumValue = 0;
	xsd__int sensorMaximumValue = 0;
	xsd__int sensorMaximumThreshold = 0;
	xsd__int sensorMinimumThreshold = 0;
	xsd__boolean sensorSMSOnOff = false;
	xsd__boolean sensorBuzzerOnOff = false;
	ns__Relay sensorRelay;
	xsd__boolean sensorLEDFlag = false;
	enum ns__SensorType sensorType = Multi;	//Multi = 0 or Mono = 1
	ns__ErrorCode sensorError;
};

class ns__Device
{
	xsd__int deviceSerialNumber = 0;
	xsd__string deviceName = "";
	xsd__string deviceFirmWareVersion = "";
	xsd__string deviceNikeName = "";
	xsd__string deviceLocation = "";
	xsd__string deviceCity = "";
	xsd__int deviceSocket = 0;
	xsd__string deviceDNSAddress;	//DNS
	ns__IP deviceIP;
	xsd__int devicePort = 0;
	xsd__dateTime deviceDateTime;
	ns__Mobile deviceMobileInfo;
	ns__Server deviceServerInfo;
	ns__Gprs deviceGprsInfo;
	std::vector<xsd__long> deviceSMSContact;
	std::vector<xsd__long> deviceSMSConfig;
	xsd__string deviceFlags = "000000000000000000";
	xsd__boolean deviceSMSCommand = false;
	xsd__boolean deviceGSMCommand = false;
	xsd__boolean deviceKeyBoardCommand = false;
	xsd__boolean deviceBuzzerOnOff = false;
	xsd__boolean deviceReset = false;
	std::vector<ns__Sensor> deviceSensors;
	ns__ErrorCode deviceError;
};

class ns__Report
{
	xsd__dateTime reportStartDate;
	xsd__dateTime reportEndDate;
};

class ns__ActivityLog
{
	ns__Report activityLogStartEndDateTime;
	xsd__string activityLogData = "";
	ns__Device activityLogDevice;
	ns__User activityLogUser;
	ns__ErrorCode activityError;
};

class ns__UserLog
{
	ns__Report userLogStartEndDateTime;
	xsd__string userLogData = "";
	ns__User userLogUser;
	ns__ErrorCode userLogError;
};

class ns__DeviceStatusLog
{
	ns__Report statusLogStartEndDateTime;
	ns__Device statusLogDevice;
	ns__ErrorCode statusLogError;
};

class ns__DeviceStatusLogList
{
	std::vector<ns__DeviceStatusLog> statusLogListStatus;
	ns__ErrorCode statusLogListError;
};

// Send to 
class ns__DeviceStatusInfo
{
	ns__Report deviceStatusInfoStartEndDateTime;
	std::vector<ns__Device> deviceStatusInfoDevice;
	std::vector<std::string> deviceStatusInfoErrorType;
};

class ns__ErrorLog
{
	xsd__dateTime errorLogDateTime;
	ns__Device errorLogDevice;
	xsd__string errorLogType = "";
	ns__ErrorCode errorLogError;
};

class ns__DeviceRules
{
	ns__Device userDeviceDevice;
	xsd__boolean userDeviceView = false;
	xsd__boolean userDeviceUpdate = false;
	xsd__boolean userDeviceDelete = false;
};

class ns__UserDevices
{
	std::vector<ns__DeviceRules> userDevices;
	ns__ErrorCode userDeviceError;
};

class ns__UserDevice
{
	ns__User userDeviceUser;
	ns__UserDevices userDeviceDevices;
	ns__ErrorCode userDeviceError;
};

class ns__DataBaseInfo
{
	xsd__string	dataBaseInfoServerName = "";
	xsd__int	dataBaseInfoPort = 0;
	xsd__string	dataBaseInfoUserName = "";
	xsd__string	dataBaseInfoPassWord = "";
	ns__ErrorCode dataBaseInfoError;
};

class ns__ServerOptions
{
	ns__DataBaseInfo serverOptionsDataBaseInfo;
	xsd__int serverOptionsTimeRepeat = 0;
	ns__ErrorCode serverOptionsError;
};

class ns__CityLocation
{
	xsd__string cityLocationCityName = "";
	std::vector<xsd__string> cityLocationLocationsName;
	ns__ErrorCode cityLocationError;
};

class ns__AllCityLocatoins
{
	std::vector<ns__CityLocation> allCityLocatoins;
	ns__ErrorCode allCityLocatoinsError;
};

xsd__int ns__GetDeviceStatus(ns__User requestUserInfo, ns__Device requestDeviceInfo, ns__Device &responseDeviceInfo);
xsd__int ns__GetAllDeviceStatus(ns__User requestUserInfo, ns__UserDevice &responseUserDevicesInfo);
xsd__int ns__SetDeviceSetting(ns__User requestUserInfo, ns__Device requestDeviceInfo, ns__ErrorCode &responseErrorInfo);
xsd__int ns__SetAllDeviceSetting(ns__User requestUserInfo, std::vector<ns__Device> requestDeviceInfo, std::vector<ns__ErrorCode> &responseErrorInfo);
xsd__int ns__DeleteDevices(ns__User requestUserInfo, std::vector<ns__Device> requestDeviceList, ns__ErrorCode &responseErrorInfo);

xsd__int ns__Login(ns__User requestUserInfo, ns__User &responseUserInfo);
xsd__int ns__GetUserDeviceList(ns__User requestUserInfo, ns__UserDevice &responseUserDevicesInfo);
xsd__int ns__GetAllDeviceList(ns__User requestUserInfo, ns__UserDevice &responseUserDevicesInfo);

xsd__int ns__GetUsersList(ns__User requestUserInfo, ns__UsersList &responseUsersList);
xsd__int ns__AddUser(ns__User requestUserInfo, ns__User requestNewUserInfo, ns__ErrorCode &responseErrorInfo);
xsd__int ns__UpdateUser(ns__User requestUserInfo, ns__User requestNewUserInfo, ns__ErrorCode &responseErrorInfo);
xsd__int ns__DeleteUser(ns__User requestUserInfo, ns__User requestDeleteUserInfo, ns__ErrorCode &responseErrorInfo);
xsd__int ns__AssignDeviceToUser(ns__User requestUserInfo, ns__UserDevice requestUserDeviceInfo, ns__ErrorCode &responseErrorInfo);
xsd__int ns__DeleteDeviceFromUserList(ns__User requestUserInfo, ns__UserDevice requestUserDeviceInfo, ns__ErrorCode &responseErrorInfo);
xsd__int ns__GetUserDevices(ns__User requestUserInfo, ns__User requestSelectedUserInfo, ns__UserDevice &responseUserDevicesInfo);

xsd__int ns__SaveActivityLog(ns__User requestUserInfo, ns__ActivityLog requestActivityLogInfo, ns__ErrorCode &responseErrorInfo);
xsd__int ns__GetActivityLog(ns__User requestUserInfo, ns__Report requestReportInfo, std::vector<ns__ActivityLog> &responseActivityLogInfo);
xsd__int ns__SaveUserLog(ns__User requestUserInfo, ns__UserLog requestActivityLogInfo, ns__ErrorCode &responseErrorInfo);
xsd__int ns__GetUserLog(ns__User requestUserInfo, ns__Report requestReportInfo, std::vector<ns__UserLog> &responseActivityLogInfo);
xsd__int ns__SaveDeviceStatusLog(ns__User requestUserInfo, ns__DeviceStatusLog requestStatusLogInfo, ns__ErrorCode &responseErrorInfo);
xsd__int ns__GetDeviceStatusLog(ns__User requestUserInfo, ns__DeviceStatusInfo requestStatusLogInfo, ns__DeviceStatusLogList &responseStatusLogList);

xsd__int ns__GetServerOptions(ns__User requestUserInfo, ns__ServerOptions &serverOptions);
xsd__int ns__SetServerOptions(ns__User requestUserInfo, ns__ServerOptions serverOptions, ns__ErrorCode &responseErrorInfo);

xsd__int ns__GetCityAndLocations(ns__User requestUserInfo, ns__AllCityLocatoins &responseCityLocationInfo);
xsd__int ns__ChangeCityName(ns__User requestUserInfo, xsd__string oldCityName, xsd__string newCityName, ns__ErrorCode &responseErrorInfo);
xsd__int ns__ChangeLocationName(ns__User requestUserInfo, xsd__string oldCityName, xsd__string newCityName, xsd__string oldLocationName, xsd__string newLocationName, ns__ErrorCode &responseErrorInfo);
xsd__int ns__ChangeDeviceName(ns__User requestUserInfo, xsd__int deviceSerialNumber, xsd__string newCityName, xsd__string newLocationName, xsd__string newDeviceName, ns__ErrorCode &responseErrorInfo);

xsd__int ns__TestWorld(xsd__dateTime requestUserInfo, xsd__dateTime &responseErrorInfo);