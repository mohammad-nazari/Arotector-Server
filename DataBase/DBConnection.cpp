#include "DBConnection.h"

DBConnection::DBConnection(void)
{
}


DBConnection::~DBConnection(void)
{
}

int DBConnection::UpdateDeviceRow(ns__Device deviceInfo)
{
	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	// Set device information
	/*db::Device dev(&conn);
	dev.SetDevsocketnumber(deviceInfo.deviceSerialNumber);
	dev.SetDevsocketnumber(deviceInfo.deviceSocket);
	dev.SetDevip(this->toolsObject.toString(deviceInfo.deviceIP.ipFirst)+"."+this->toolsObject.toString(deviceInfo.deviceIP.ipSecond)+"."+this->toolsObject.toString(deviceInfo.deviceIP.ipThird)+"."+this->toolsObject.toString(deviceInfo.deviceIP.ipFourth));
	dev.SetDevport(deviceInfo.devicePort);
	dev.SetDevstatus('1');
	dev.SetDevsimcardnumber(deviceInfo.deviceMobileInfo.mobileNumber);
	dev.SetDevsimcardtype(this->toolsObject.SimCardTypeToString(deviceInfo.deviceMobileInfo.mobileSimCardType));

	// Update device information and return result
	dev.update();

	return 1;*/

	Query q(conn);

	std::string query = UPDATEDEVICE;

	size_t position = 0;



	position = this->toolsObject.replaceOne(query, "%s", this->toolsObject.toString(deviceInfo.deviceSocket), position); //DevSocket
	position = this->toolsObject.replaceOne(query, "%s", this->toolsObject.toString(deviceInfo.deviceIP.ipFirst) + "." + this->toolsObject.toString(deviceInfo.deviceIP.ipSecond) + "." + this->toolsObject.toString(deviceInfo.deviceIP.ipThird) + "." + this->toolsObject.toString(deviceInfo.deviceIP.ipFourth), position); //DevIP
	position = this->toolsObject.replaceOne(query, "%s", this->toolsObject.toString(deviceInfo.devicePort), position); //DevPort
	position = this->toolsObject.replaceOne(query, "%s", "1", position); //DevStatus
	position = this->toolsObject.replaceOne(query, "%s", this->toolsObject.toString(deviceInfo.deviceMobileInfo.mobileNumber), position); //DevSimcardNumber
	// Convert enum to string save to database
	position = this->toolsObject.replaceOne(query, "%s", this->toolsObject.SimCardTypeToString(deviceInfo.deviceMobileInfo.mobileSimCardType), position); //DevSimcardType

	position = this->toolsObject.replaceOne(query, "%s", this->toolsObject.toString(deviceInfo.deviceSerialNumber), position); //DevSerialNumber

	if(!(q.execute(query)))
	{
		return 0; // Error occurred
	}

	return 1;
}

int DBConnection::CreateDeviceRow(ns__Device deviceInfo)
{
	// save connection error
	StderrLog log;

	// Generate and connect
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	// Set device information
	db::Device dev(&conn);
	dev.SetDevserialnumber(deviceInfo.deviceSerialNumber);
	dev.SetDevcity("NoCity");
	dev.SetDevlocation("NoLocation");
	dev.SetDevname(this->toolsObject.toString(deviceInfo.deviceSerialNumber));
	dev.SetDevsocketnumber(deviceInfo.deviceSocket);
	dev.SetDevip(this->toolsObject.toString(deviceInfo.deviceIP.ipFirst) + "." + this->toolsObject.toString(deviceInfo.deviceIP.ipSecond) + "." + this->toolsObject.toString(deviceInfo.deviceIP.ipThird) + "." + this->toolsObject.toString(deviceInfo.deviceIP.ipFourth));
	dev.SetDevport(deviceInfo.devicePort);
	dev.SetDevstatus('1');
	dev.SetDevsimcardnumber(deviceInfo.deviceMobileInfo.mobileNumber);
	// Convert enum to string save to database
	dev.SetDevsimcardtype(this->toolsObject.SimCardTypeToString(deviceInfo.deviceMobileInfo.mobileSimCardType));

	// Add new device into database and return result
	return dev.insert();
}

int DBConnection::CheckSerialNumber(std::string Input)
{
	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	Query q(conn);

	std::string query = SELECTDEVICESERIAL;

	this->toolsObject.replaceOne(query, "%s", Input, 0);

	q.get_result(query);
	if(q.num_rows() < 1)
	{
		q.free_result();
		return 0; //Device Serial Number Does not exist
	}
	q.free_result();

	return 1;
}

//************************************
// Method:    CheckUser
// FullName:  DBConnection::CheckUser
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: ns__User
//
// Description: Generate a connection to data base
//				check information of user
//************************************
int DBConnection::CheckUser(ns__User &UserInfo)
{
	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	// Create object and set username and password to check
	db::Users userObj(conn, UserInfo.userName, UserInfo.userPassword);
	if(userObj.GetUserid() > 0)
	{
		UserInfo.userId = userObj.GetUserid();
		UserInfo.userFirstName = userObj.GetUserfirstname();
		UserInfo.userLastName = userObj.GetUserlastname();
		UserInfo.userName = userObj.GetUsername();
		// Convert string to enum
		this->toolsObject.UserTypeFromString(UserInfo.userType, userObj.GetUsertype());
	}
	// return result
	return userObj.GetUserid();
}

//************************************
// Method:    ConnectToDatabase
// FullName:  DBConnection::ConnectToDatabase
// Access:    private 
// Returns:   bool
// Qualifier:
//
// Description: Set a Connection to database
//************************************
bool DBConnection::ConnectToDatabase()
{
	this->connection = new Database(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &(this->log));
	if(!this->connection->Connected())
	{
		return false; // DATABASE Connection Error
	}

	return true;
}

int DBConnection::CheckDevice(ns__Device DeviceInfo)
{
	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	// Create object and set username and password to check
	db::Device deviceObj(conn, DeviceInfo.deviceSerialNumber);

	// return result
	return deviceObj.GetDevserialnumber();
}

int DBConnection::CheckUserDevice(ns__User UserInfo, ns__Device DeviceInfo, int AllowType/*=0*/)
{
	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	// Create object and set username and password to check
	db::Userdevice userDeviceObj(conn, UserInfo.userId, DeviceInfo.deviceSerialNumber);

	// Check for write to device allowing
	if(AllowType == 1)
	{
		if(userDeviceObj.GetId() != 0 && userDeviceObj.GetUpdateact() == 0)
		{
			// User Not allow to Write to device
			return -2;
		}
	}

	// return result
	return userDeviceObj.GetId();
}

int DBConnection::GetUserDeviceList(ns__User UserInfo, std::vector<ns__DeviceRules> &DeviceList)
{
	ns__DeviceRules deviceTemp;

	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	Query q(conn);

	if(this->GetUserInfo(UserInfo) > 0)
	{
		// String of query
		std::string strQuery = SELECTUSERDEVICELIST;

		// Replace Values
		this->toolsObject.replaceOne(strQuery, "%s", this->toolsObject.toString(UserInfo.userId), 0);

		q.get_result(strQuery);
		if(q.num_rows() < 1)
		{
			q.free_result();
			return 0; //Device Serial Number Does not exist
		}

		// Reset device list
		DeviceList.clear();

		// Get device info in loop
		while(q.fetch_row())
		{
			db::Userdevice tbl(&conn, &q); // spawns an object from Query object

			if(GetDeviceInfo(tbl.GetDeviceid(), deviceTemp) == 1)
			{
				deviceTemp.userDeviceView = tbl.GetViewact();
				deviceTemp.userDeviceUpdate = tbl.GetUpdateact();
				deviceTemp.userDeviceDelete = tbl.GetDeleteact();
				DeviceList.push_back(deviceTemp);
			}
		}

		q.free_result();

		return 1;
	}

	return 0;
}

int DBConnection::GetDeviceInfo(int DeviceId, ns__DeviceRules& DeviceInformation)
{
	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // MYSQL Connection Error
	}

	Query q(conn);

	std::string query = SELECTDEVICEINFO;

	this->toolsObject.replaceOne(query, "%s", this->toolsObject.toString(DeviceId), 0);

	q.get_result(query);
	if(q.num_rows() < 1)
	{
		q.free_result();
		return 0; // User name and or password is incorrect
	}

	q.fetch_row();
	db::Device tbl(&conn, &q); // spawns an object from Query object
	DeviceInformation.userDeviceDevice.deviceSerialNumber = tbl.GetDevserialnumber();
	DeviceInformation.userDeviceDevice.deviceCity = tbl.GetDevcity();
	DeviceInformation.userDeviceDevice.deviceLocation = tbl.GetDevlocation();
	DeviceInformation.userDeviceDevice.deviceNikeName = tbl.GetDevname();

	q.free_result();

	return 1;
}

std::vector<ns__DeviceRules> DBConnection::getUDeviceList()
{
	return this->userDeviceList;
}

void DBConnection::setUserDeviceList(std::vector<ns__Device> UserDeviceList)
{
	this->userDeviceList = userDeviceList;
}

int DBConnection::GetDeviceSetting(ns__Device& DeviceInfo)
{
	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	// Create object and set username and password to check
	db::Device deviceObj(conn, DeviceInfo.deviceSerialNumber);

	if(deviceObj.GetDevserialnumber() > 0)
	{
		// Fill Device info
		Splitter sp(deviceObj.GetDevip(), ".");
		if(sp.size() == 4)
		{
			DeviceInfo.deviceIP.ipFirst = this->toolsObject.toInteger(sp[0]);
			DeviceInfo.deviceIP.ipSecond = this->toolsObject.toInteger(sp[1]);
			DeviceInfo.deviceIP.ipThird = this->toolsObject.toInteger(sp[2]);
			DeviceInfo.deviceIP.ipFourth = this->toolsObject.toInteger(sp[3]);
		}
		DeviceInfo.devicePort = deviceObj.GetDevport();
		DeviceInfo.deviceCity = deviceObj.GetDevcity();
		DeviceInfo.deviceLocation = deviceObj.GetDevlocation();
		DeviceInfo.deviceNikeName = deviceObj.GetDevname();

		return 1;
	}

	return 0;
}

int DBConnection::UpdateDeviceRowSetting(ns__Device deviceInfo)
{
	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	/*// Set device information
	db::Device dev(&conn);

	dev.SetDevcity( deviceInfo.deviceCity );
	dev.SetDevlocation( deviceInfo.deviceLocation );
	dev.SetDevname( deviceInfo.deviceNikeName );

	// Update device information and return result
	dev.update();

	return 1;*/

	Query q(conn);

	std::string query = UPDATEDEVICESETTING;

	size_t position = 0;

	position = this->toolsObject.replaceOne(query, "%s", deviceInfo.deviceCity, position); //DevCity
	position = this->toolsObject.replaceOne(query, "%s", deviceInfo.deviceLocation, position); //DevLocation
	position = this->toolsObject.replaceOne(query, "%s", deviceInfo.deviceNikeName, position); //DevName

	position = this->toolsObject.replaceOne(query, "%s", this->toolsObject.toString(deviceInfo.deviceSerialNumber), position); //DevSerialNumber

	if(!(q.execute(query)))
	{
		return 0; // Error occurred
	}

	return 1;
}

int DBConnection::GetAllDeviceList()
{
	ns__DeviceRules deviceTemp;

	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // MYSQL Connection Error
	}

	Query q(conn);

	std::string query = SELECTDEVICEINFOALL;

	q.get_result(query);
	if(q.num_rows() < 1)
	{
		q.free_result();
		return 0; // User name and or password is incorrect
	}

	while(q.fetch_row())
	{
		db::Device tbl(&conn, &q); // spawns an object from Query object
		deviceTemp.userDeviceDevice.deviceSerialNumber = tbl.GetDevserialnumber();
		deviceTemp.userDeviceDevice.deviceCity = tbl.GetDevcity();
		deviceTemp.userDeviceDevice.deviceLocation = tbl.GetDevlocation();
		deviceTemp.userDeviceDevice.deviceNikeName = tbl.GetDevname();

		this->userDeviceList.push_back(deviceTemp);
	}
	q.free_result();

	return 1;
}

int DBConnection::SaveDeviceStatus(ns__Device DeviceObject, std::string DeviceStatusString)
{
	// save connection error
	StderrLog log;

	// Generate and connect
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	// Set device information
	db::Logstatus logStatus(&conn);
	logStatus.SetDevserialnumber(DeviceObject.deviceSerialNumber);
	logStatus.SetLogdate(this->toolsObject.getCurrentDateTime());
	logStatus.SetStatusdata(DeviceStatusString);

	// Add new device into database and return result
	return logStatus.insert();
}

//************************************
// Method:    GetAllCityLoactionName
// FullName:  DBConnection::GetAllCityLoactionName
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: ns__AllCityLocatoins & AllCityLocationName
//
// Descriptions: Get all city with locations
//************************************
int DBConnection::GetAllCityLoactionName(ns__AllCityLocatoins &AllCityLocationName)
{
	ns__CityLocation cityLocation;
	std::vector<std::string> cities;
	// First get all city name
	if(this->GetCitiesName(cities) > 0)
	{
		for each (std::string cityName in cities)
		{
			// Get all locations in this city
			cityLocation.cityLocationCityName = cityName;

			if(this->GetLacationsName(cityName, cityLocation.cityLocationLocationsName) > 0)
			{
				AllCityLocationName.allCityLocatoins.push_back(cityLocation);
			}
		}
		return 1;
	}
	return 0;
}

int DBConnection::GetCitiesName(std::vector<std::string> &AllCityName)
{
	AllCityName.clear();

	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // MYSQL Connection Error
	}

	Query q(conn);

	std::string query = SELECTCITYNAME;

	q.get_result(query);
	if(q.num_rows() < 1)
	{
		q.free_result();
		return 0;
	}

	while(q.fetch_row())
	{
		//db::Device tbl(&conn, &q); // spawns an object from Query object
		//AllCityName.push_back(tbl.GetDevcity());
		AllCityName.push_back(q.getstr(0));
	}
	q.free_result();

	return 1;
}

int DBConnection::GetLacationsName(std::string CityName, std::vector<std::string> &AllLocationName)
{
	AllLocationName.clear();

	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // MYSQL Connection Error
	}

	Query q(conn);

	std::string query = SELECTLOCATIONNAME;

	this->toolsObject.replaceAll(query, "%s", CityName);

	q.get_result(query);
	if(q.num_rows() < 1)
	{
		q.free_result();
		return 0;
	}

	while(q.fetch_row())
	{
		//db::Device tbl(&conn, &q); // spawns an object from Query object
		//AllLocationName.push_back(tbl.GetDevlocation());
		AllLocationName.push_back(q.getstr(0));
	}
	q.free_result();

	return 1;
}

int DBConnection::ChangeCityName(std::string OldCityName, std::string NewCityName)
{
	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	Query q(conn);

	std::string query = UPDATECITYNAME;

	size_t position = 0;
	position = this->toolsObject.replaceOne(query, "%s", NewCityName, position); //New city name
	position = this->toolsObject.replaceOne(query, "%s", OldCityName, position); //Old city name

	if(!(q.execute(query)))
	{
		return 0; // Error occurred
	}

	return 1;
}

int DBConnection::ChangeLocationName(std::string OldCityName, std::string NewCityName, std::string OldLocationName, std::string NewLocationName)
{
	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	Query q(conn);

	std::string query = UPDATELOCATIONNAME;

	size_t position = 0;
	position = this->toolsObject.replaceOne(query, "%s", NewLocationName, position); //New location name
	position = this->toolsObject.replaceOne(query, "%s", NewCityName, position); //New location name
	position = this->toolsObject.replaceOne(query, "%s", OldCityName, position); //Location city name
	position = this->toolsObject.replaceOne(query, "%s", OldLocationName, position); //Old location name

	if(!(q.execute(query)))
	{
		return 0; // Error occurred
	}

	return 1;
}

int DBConnection::ChangeDeviceName(int DeviceSerialNumber, std::string NewCityName, std::string NewLocationName, std::string NewDeviceName)
{
	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	Query q(conn);

	std::string query = UPDATEDEVICENAME;

	size_t position = 0;
	position = this->toolsObject.replaceOne(query, "%s", NewDeviceName, position); //New device name
	position = this->toolsObject.replaceOne(query, "%s", NewLocationName, position); //New device location name
	position = this->toolsObject.replaceOne(query, "%s", NewCityName, position); //New device city name
	position = this->toolsObject.replaceOne(query, "%s", this->toolsObject.toString(DeviceSerialNumber), position); //Device serial number

	if(!(q.execute(query)))
	{
		return 0; // Error occurred
	}

	return 1;
}

int DBConnection::GetUsersList(std::vector<ns__User> &UserList)
{
	ns__DeviceRules deviceTemp;

	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	Query q(conn);

	// String of query
	std::string strQuery = SELECTALLUSERS;

	q.get_result(strQuery);
	if(q.num_rows() < 1)
	{
		q.free_result();
		return 0; //Device Serial Number Does not exist
	}

	// Reset User list
	UserList.clear();

	ns__User userTemp;
	// Get device info in loop
	while(q.fetch_row())
	{
		db::Users tbl(&conn, &q); // spawns an object from Query object
		if(tbl.GetUsername() != "admin")
		{
			userTemp.userId = tbl.GetUserid();
			userTemp.userName = tbl.GetUsername();
			userTemp.userFirstName = tbl.GetUserfirstname();
			userTemp.userLastName = tbl.GetUserlastname();
			//userTemp.userSubmitDateTime = std::asctime(tbl.GetRegisterdate());
			this->toolsObject.UserTypeFromString(userTemp.userType, tbl.GetUsertype());

			UserList.push_back(userTemp);
		}
	}

	q.free_result();

	return 1;
}

int DBConnection::AddUser(ns__User UserInfo)
{
	// save connection error
	StderrLog log;

	// Generate and connect
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	// Set device information
	db::Users usr(&conn);
	usr.SetUsername(UserInfo.userName);
	usr.SetPassword(UserInfo.userPassword);
	usr.SetUserfirstname(UserInfo.userFirstName);
	usr.SetUserlastname(UserInfo.userLastName);
	usr.SetUsertype(this->toolsObject.UserTypeToString(UserInfo.userType));
	usr.SetRegisterdate(toolsObject.getCurrentDateTime());
	usr.SetStatus("Logout");

	// Add new device into database and return result
	return usr.insert();
}

int DBConnection::DeleteUser(ns__User UserInfo)
{
	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	if(this->GetUserInfo(UserInfo) > 0)
	{
		Query q(conn);

		std::string query = DELETEUSER;

		size_t position = 0;
		position = this->toolsObject.replaceOne(query, "%s", this->toolsObject.toString(UserInfo.userId), position); //User ID

		if(!(q.execute(query)))
		{
			return 0; // Error occurred
		}

		return 1;
	}

	return 0;
}

int DBConnection::AssignDeviceToUser(ns__UserDevice UserDeviceList)
{
	// save connection error
	StderrLog log;

	// Generate and connect
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	if(this->GetUserInfo(UserDeviceList.userDeviceUser) > 0)
	{
		for each (ns__DeviceRules deviceRule in UserDeviceList.userDeviceDevices.userDevices)
		{
			// Set device information
			db::Userdevice usrDev(&conn);
			usrDev.SetUserid(UserDeviceList.userDeviceUser.userId);
			usrDev.SetDeviceid(deviceRule.userDeviceDevice.deviceSerialNumber);
			usrDev.SetDeleteact(deviceRule.userDeviceDelete);
			usrDev.SetUpdateact(deviceRule.userDeviceUpdate);
			usrDev.SetViewact(deviceRule.userDeviceView);
			usrDev.SetAssigndate(toolsObject.getCurrentDateTime());

			// Add new device into database and return result
			usrDev.insert();
		}
		return 1;
	}
	return 0;
}

int DBConnection::DeleteDeviceFromUser(ns__UserDevice UserDeviceList)
{
	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	if(this->GetUserInfo(UserDeviceList.userDeviceUser) > 0)
	{
		Query q(conn);

		std::string query = "";

		size_t position = 0;

		for each (ns__DeviceRules deviceRule in UserDeviceList.userDeviceDevices.userDevices)
		{
			query = DELETEUSERDEVICE;

			position = 0;

			position = this->toolsObject.replaceOne(query, "%s", this->toolsObject.toString(UserDeviceList.userDeviceUser.userId), position); //User ID
			position = this->toolsObject.replaceOne(query, "%s", this->toolsObject.toString(deviceRule.userDeviceDevice.deviceSerialNumber), position); //Device ID
			try
			{
				q.execute(query);
			}
			catch(std::exception e)
			{
				log.error(conn, e.what());
				return 0;
			}
		}

		return 1;
	}
	return 0;
}

int DBConnection::UpdateUser(ns__User UserInfo)
{
	// save connection error
	StderrLog log;

	// Generate and connect
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}
	ns__User temp = UserInfo;

	if(this->GetUserInfo(temp) > 0)
	{
		UserInfo.userId = temp.userId;
		// Set device information
		db::Users usr(&conn);
		usr.SetUserid(UserInfo.userId);
		usr.SetUsername(UserInfo.userName);
		usr.SetPassword(UserInfo.userPassword);
		usr.SetUserfirstname(UserInfo.userFirstName);
		usr.SetUserlastname(UserInfo.userLastName);
		usr.SetUsertype(this->toolsObject.UserTypeToString(UserInfo.userType));
		usr.SetStatus("Login");

		try
		{
			// Add new device into database and return result
			usr.update();

			return 1;
		}
		catch(std::exception e)
		{
			log.error(conn, e.what());
			return 0;
		}
	}

	return 0;
}

int DBConnection::GetUserInfo(ns__User& UserInformation)
{
	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	Query q(conn);

	// String of query
	std::string strQuery = SELECTUSERWITHNAME;
	size_t position = 0;
	position = this->toolsObject.replaceOne(strQuery, "%s", UserInformation.userName, position); //New device name

	q.get_result(strQuery);
	if(q.num_rows() < 1)
	{
		q.free_result();
		return 0; //User Does not exist
	}

	if(q.fetch_row())
	{
		// Create object and set username and password to check
		db::Users userObj(&conn, &q);
		if(userObj.GetUserid() > 0)
		{
			UserInformation.userId = userObj.GetUserid();
			UserInformation.userFirstName = userObj.GetUserfirstname();
			UserInformation.userLastName = userObj.GetUserlastname();
			// Convert string to enum
			this->toolsObject.UserTypeFromString(UserInformation.userType, userObj.GetUsertype());
		}

		q.free_result();

		// return result
		return userObj.GetUserid();
	}

	return 0;
}

int DBConnection::GetDeviceStatusLog(ns__DeviceStatusInfo StatusLogInfo, ns__DeviceStatusLogList &StatusLogList)
{
	int result = 0;
	// save connection error
	StderrLog log;
	Database conn(DATABASESERVER, DATABASEUSER, DATABASEPASSWORD, DATABASENAME, &log);
	if(!conn.Connected())
	{
		return -1; // DATABASE Connection Error
	}

	Query q(conn);

	// String of query
	std::string strQuery = SELECTDEVICESTATUSLOG;
	size_t position = 0;

	char strDateTime[30];
	std::strftime(strDateTime, 100, "%Y-%m-%d %H:%M:%S", (const tm*)(&(StatusLogInfo.deviceStatusInfoStartEndDateTime.reportStartDate)));
	position = this->toolsObject.replaceOne(strQuery, "%s", strDateTime, position); //Start Date

	std::strftime(strDateTime, 100, "%Y-%m-%d %H:%M:%S", (const tm*)(&(StatusLogInfo.deviceStatusInfoStartEndDateTime.reportEndDate)));
	position = this->toolsObject.replaceOne(strQuery, "%s", strDateTime, position); //End Date

	size_t iSize = StatusLogInfo.deviceStatusInfoErrorType.size();
	std::string strTemp = "";
	for(int i = 0; i < iSize; i++)
	{
		strTemp += StatusLogInfo.deviceStatusInfoErrorType[i];
		if(i != iSize - 1)
		{
			strTemp += "|";
		}
	}
	position = this->toolsObject.replaceOne(strQuery, "%s", strTemp, position); //New device name

	strTemp = "";
	iSize = StatusLogInfo.deviceStatusInfoDevice.size();
	for(int i = 0; i < iSize; i++)
	{
		strTemp += this->toolsObject.toString(StatusLogInfo.deviceStatusInfoDevice[i].deviceSerialNumber);
		if(i != iSize - 1)
		{
			strTemp += ",";
		}
	}
	position = this->toolsObject.replaceOne(strQuery, "%s", strTemp, position); //New device name

	q.get_result(strQuery);
	if(q.num_rows() < 1)
	{
		q.free_result();
		return 0; //User Does not exist
	}

	result = q.num_rows();

	while(q.fetch_row())
	{
		// Create object and set username and password to check
		db::Logstatus statusLogObj(&conn, &q);
		db::Device deviceObj(&conn, &q, statusLogObj.num_cols());
		if(deviceObj.GetDevserialnumber() > 0)
		{
			ns__DeviceStatusLog devStatusTemp;
			devStatusTemp.statusLogDevice.deviceSerialNumber = deviceObj.GetDevserialnumber();
			devStatusTemp.statusLogDevice.deviceCity = deviceObj.GetDevcity();
			devStatusTemp.statusLogDevice.deviceLocation = deviceObj.GetDevlocation();
			devStatusTemp.statusLogDevice.deviceName = deviceObj.GetDevname();
			devStatusTemp.statusLogDevice.deviceNikeName = deviceObj.GetDevname();

			devStatusTemp.statusLogError.errorMessage = statusLogObj.GetStatusdata();
			devStatusTemp.statusLogStartEndDateTime.reportStartDate = this->toolsObject.toTMStruct(statusLogObj.GetLogdate());

			StatusLogList.statusLogListStatus.push_back(devStatusTemp);
		}
	}

	q.free_result();

	return result;
}
