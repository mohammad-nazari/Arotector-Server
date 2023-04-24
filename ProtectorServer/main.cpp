
#pragma warning(disable: 4146) // warning C4146: unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable: 4624) // warning C4624: 'llvm::AugmentedUse' : destructor could not be generated because a base class destructor is inaccessible
#pragma warning(disable: 4355) // warning C4355: 'this' : used in base member initializer list
#pragma warning(disable: 4800) // warning C4800: 'const unsigned int' : forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable: 4996) // warning C4996: 'std::_Copy_impl': Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. To disable this warning, use -D_Sclan::SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'
#pragma warning(disable: 4244) // warning C4244: 'return' : conversion from 'uint64_t' to 'unsigned int', possible loss of data

#include "Server_SoapH.h"
#include "Protector.nsmap"

#include "Header.h"
#include "ExecuteRequest.h"
#include "SConnection.h"

// Global Device List
std::vector<ns__Device> deviceInfoList;
std::map<int, DeviceMap> deviceMap;

// A global socket object
SConnection SocketObject;

void start_socket_listener()
{
	SConnection SocketObject1;
	// Create socket
	SocketObject1.CreateSocket();
	// Start listen server to socket and continue
	SocketObject1.ThreadStartListen();
}

int main(int argc, char **argv)
{
	// Start server listen to devices
	start_socket_listener();

	struct soap soap;   // Generate soap object
	soap_init(&soap);   // Initialize
	soap.namespaces = new struct Namespace;
	soap.namespaces = namespaces;

	//if( argc < 2 ) // no args: assume this is a CGI application
	//{
	//	soap_serve( &soap ); // serve request, one thread, CGI style
	//	soap_destroy( &soap ); // deallocated C++ data
	//	soap_end( &soap ); // deallocated data and clean up
	//}
	//else
	{
		soap.send_timeout = 5; // 60 seconds
		soap.recv_timeout = 5; // 60 seconds
		//        soap.accept_timeout = 3600; // server stops after 1 hour of inactivity (never stop)
		soap.max_keep_alive = BACKLOG; // max keep-alive sequence
		struct soap *tsoap;
		//int port = atoi(argv[1]); // first command-line arg is port
		int port = 9090;
		SOAP_SOCKET m, s;
		m = soap_bind(&soap, NULL, port, BACKLOG);
		if(!soap_valid_socket(m))
			exit(1);
		fprintf(stderr, "Socket connection successful %d\n", m);
		for(;;)
		{
			s = soap_accept(&soap);
			if(!soap_valid_socket(s))
			{
				if(soap.errnum)
				{
					soap_print_fault(&soap, stderr);
					exit(1);
				}
				fprintf(stderr, "server timed out\n");
				break;
			}
			tsoap = soap_copy(&soap); // make a safe copy
			if(!tsoap)
				break;

			if(soap_serve(&soap) != SOAP_OK)	// process RPC request 
				soap_print_fault(&soap, stderr); // print error

			printf("Thread of socket %d connection from IP %d.%d.%d.%d served\n", s, (soap.ip >> 24) & 0xFF, (soap.ip >> 16) & 0xFF, (soap.ip >> 8) & 0xFF, soap.ip & 0xFF);

			soap_destroy(tsoap); // deallocated C++ data
			soap_end(tsoap); // deallocated data and clean up
			soap_done(tsoap); // detach soap struct
			free(tsoap);
		}
	}
	soap_done(&soap); // detach soap struct
	return 0;
}

xsd__int ns__GetDeviceStatus(soap* soap, ns__User requestUserInfo, ns__Device requestDeviceInfo, ns__Device &responseDeviceInfo)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;

	// Initialize response
	responseDeviceInfo = requestDeviceInfo;
	soap_default_tm(soap, &(responseDeviceInfo.deviceDateTime));

	// User Authentication checking
	requestUserInfo.userId = erObject.AuthenticateUser(requestUserInfo);
	if(requestUserInfo.userId > 0)
	{
		// Check device is in database
		if(erObject.CheckDevice(requestDeviceInfo) > 0)
		{
			int result = 0;
			if(requestUserInfo.userType == ns__UserType::Admin)
			{
				result = 1;
			}
			else
			{
				// Check device is in list of user devices
				// and can view status
				result = erObject.IsDeviceForUser(requestUserInfo, requestDeviceInfo, 1 /* AllowType = 1 allow to write*/);
			}

			if(result > 0)
			{
				// Get device Status
				if(!(SocketObject.GetDeviceStatus(responseDeviceInfo)))
				{
					responseDeviceInfo.deviceError.errorMessage = NOCONNECTEDMESSAGE;
					responseDeviceInfo.deviceError.errorNumber = NOCONNECTEDNUMBER;

					soap_default_tm(soap, &(responseDeviceInfo.deviceDateTime));
				}

				// Get other device information from database
				if(erObject.GetDeviceSetting(responseDeviceInfo) == 1)
				{
					erObject.DeviceInfoToBase64(responseDeviceInfo);
				}
				else
				{
					// Database Error
					responseDeviceInfo.deviceError.errorMessage = DATABASEERRORTMESSAGE;
					responseDeviceInfo.deviceError.errorNumber = DATABASEERRORNUMBER;
				}
			}
			else
			{
				if(result == -2)
				{
					// Permission Denied
					// Fill error in response
					responseDeviceInfo.deviceError.errorMessage = DEVICEPERMISSIONMESSAGE;
					responseDeviceInfo.deviceError.errorNumber = DEVICEPERMISSIONNUMBER;
				}
				else
				{
					// Database Error
					responseDeviceInfo.deviceError.errorMessage = DATABASEERRORTMESSAGE;
					responseDeviceInfo.deviceError.errorNumber = DATABASEERRORNUMBER;
				}
			}
		}
		else
		{
			// Fill error in response
			responseDeviceInfo.deviceError.errorMessage = DEVICEEXISTMESSAGE;
			responseDeviceInfo.deviceError.errorNumber = DEVICEEXISTNUMBER;
		}
	}
	else
	{
		// Fill error in response
		responseDeviceInfo.deviceError.errorMessage = USERAUTHENTICATIONMESSAGE;
		responseDeviceInfo.deviceError.errorNumber = USERAUTHENTICATIONNUMBER;
	}

	return SOAP_OK;
}

xsd__int ns__GetAllDeviceStatus(soap* soap, ns__User requestUserInfo, ns__UserDevice &responseUserDevicesInfo)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;
	soap_default_tm(soap, &(requestUserInfo.userSubmitDateTime));

	soap_default_tm(soap, &(responseUserDevicesInfo.userDeviceUser.userSubmitDateTime));

	// User Authentication checking
	requestUserInfo.userId = erObject.AuthenticateUser(requestUserInfo);
	if(requestUserInfo.userId > 0)
	{
		if(requestUserInfo.userType == ns__UserType::Admin)
		{
			if(erObject.GetAllDeviceList(responseUserDevicesInfo.userDeviceDevices.userDevices) != 1)
			{
				// No device found error
				responseUserDevicesInfo.userDeviceError.errorMessage = NODEVICEFOUNDMESSAGE;
				responseUserDevicesInfo.userDeviceError.errorNumber = NODEVICEFOUNDNUMBER;
			}
			else
			{
				for(int i = 0; i < responseUserDevicesInfo.userDeviceDevices.userDevices.size(); i++)
				{
					soap_default_tm(soap, &(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice.deviceDateTime));
					// Get device Status
					if(!(SocketObject.GetDeviceStatus(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice)))
					{
						responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice.deviceError.errorMessage = NOCONNECTEDMESSAGE;
						responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice.deviceError.errorNumber = NOCONNECTEDNUMBER;
					}

					// Get other device information from database
					if(erObject.GetDeviceSetting(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice) == 1)
					{
						erObject.DeviceInfoToBase64(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice);
					}
					else
					{
						// Database Error
						responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice.deviceError.errorMessage = DATABASEERRORTMESSAGE;
						responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice.deviceError.errorNumber = DATABASEERRORNUMBER;
					}
				}
			}
		}
		else
		{
			if(erObject.GetUserDeviceList(requestUserInfo, responseUserDevicesInfo.userDeviceDevices.userDevices) != 1)
			{
				// No device found error
				responseUserDevicesInfo.userDeviceError.errorMessage = NODEVICEFOUNDMESSAGE;
				responseUserDevicesInfo.userDeviceError.errorNumber = NODEVICEFOUNDNUMBER;
			}
			else
			{
				for(int i = 0; i < responseUserDevicesInfo.userDeviceDevices.userDevices.size(); i++)
				{
					soap_default_tm(soap, &(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice.deviceDateTime));
					if(!(SocketObject.GetDeviceStatus(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice)))
					{
						responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice.deviceError.errorMessage = NOCONNECTEDMESSAGE;
						responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice.deviceError.errorNumber = NOCONNECTEDNUMBER;
					}

					// Get other device information from database
					if(erObject.GetDeviceSetting(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice) == 1)
					{
						erObject.DeviceInfoToBase64(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice);
					}
					else
					{
						// Database Error
						responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice.deviceError.errorMessage = DATABASEERRORTMESSAGE;
						responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice.deviceError.errorNumber = DATABASEERRORNUMBER;
					}
				}
			}
		}
	}
	else
	{
		// Fill error in response
		responseUserDevicesInfo.userDeviceError.errorMessage = USERAUTHENTICATIONMESSAGE;
		responseUserDevicesInfo.userDeviceError.errorNumber = USERAUTHENTICATIONNUMBER;
	}

	return SOAP_OK;
}

xsd__int ns__SetDeviceSetting(soap* soap, ns__User requestUserInfo, ns__Device requestDeviceInfo, ns__ErrorCode &responseErrorInfo)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;

	erObject.DeviceInfoFromBase64(requestDeviceInfo);

	// User Authentication checking
	requestUserInfo.userId = erObject.AuthenticateUser(requestUserInfo);
	if(requestUserInfo.userId > 0)
	{
		// Check device is in database
		if(erObject.CheckDevice(requestDeviceInfo) > 0)
		{
			int result = 0;
			if(requestUserInfo.userType == ns__UserType::Admin)
			{
				result = 1;
			}
			else
			{
				// Check device is in list of user devices
				// and can view status
				result = erObject.IsDeviceForUser(requestUserInfo, requestDeviceInfo, 1 /* AllowType = 1 allow to write*/);
			}
			if(result > 0)
			{
				// Set setting in device
				if(SocketObject.SetDeviceStatus(requestDeviceInfo))
				{
					// Set setting in database
					if(erObject.SetDeviceSetting(requestDeviceInfo))
					{

					}
					else
					{
						// Database Error
						responseErrorInfo.errorMessage = DATABASEERRORTMESSAGE;
						responseErrorInfo.errorNumber = DATABASEERRORNUMBER;
					}
				}
				else
				{
					// Fill error in response
					responseErrorInfo.errorMessage = DEVICEEXISTMESSAGE;
					responseErrorInfo.errorNumber = DEVICEEXISTNUMBER;
				}
			}
			else
			{
				if(result == -2)
				{
					// Permission Denied
					// Fill error in response
					responseErrorInfo.errorMessage = DEVICEPERMISSIONMESSAGE;
					responseErrorInfo.errorNumber = DEVICEPERMISSIONNUMBER;
				}
				else
				{
					// Database Error
					responseErrorInfo.errorMessage = DATABASEERRORTMESSAGE;
					responseErrorInfo.errorNumber = DATABASEERRORNUMBER;
				}
			}
		}
		else
		{
			// Fill error in response
			responseErrorInfo.errorMessage = DEVICEEXISTMESSAGE;
			responseErrorInfo.errorNumber = DEVICEEXISTNUMBER;
		}
	}
	else
	{
		// Fill error in response
		responseErrorInfo.errorMessage = USERAUTHENTICATIONMESSAGE;
		responseErrorInfo.errorNumber = USERAUTHENTICATIONNUMBER;
	}

	return SOAP_OK;
}

xsd__int ns__SetAllDeviceSetting(soap* soap, ns__User requestUserInfo, std::vector<ns__Device> requestDevicesInfo, std::vector<ns__ErrorCode> &responseErrorInfo)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;
	ns__ErrorCode errorTemp;

	// User Authentication checking
	requestUserInfo.userId = erObject.AuthenticateUser(requestUserInfo);
	if(requestUserInfo.userId > 0)
	{
		for each (ns__Device requestDeviceInfo in requestDevicesInfo)
		{
			// Check device is in database
			if(erObject.CheckDevice(requestDeviceInfo) > 0)
			{
				// Check device is in list of user devices
				// and can view status
				int result = erObject.IsDeviceForUser(requestUserInfo, requestDeviceInfo, 1 /* AllowType = 1 allow to write*/);
				if(result > 0)
				{
					// Set setting in device
					if(SocketObject.SetDeviceStatus(requestDeviceInfo))
					{
						// Set setting in database
						if(erObject.SetDeviceSetting(requestDeviceInfo))
						{

						}
						else
						{
							// Database Error
							errorTemp.errorMessage = DATABASEERRORTMESSAGE;
							errorTemp.errorNumber = DATABASEERRORNUMBER;
						}
					}
					else
					{
						// Fill error in response
						errorTemp.errorMessage = DEVICEEXISTMESSAGE;
						errorTemp.errorNumber = DEVICEEXISTNUMBER;
					}
				}
				else
				{
					if(result == -2)
					{
						// Permission Denied
						// Fill error in response
						errorTemp.errorMessage = DEVICEPERMISSIONMESSAGE;
						errorTemp.errorNumber = DEVICEPERMISSIONNUMBER;
					}
					else
					{
						// Database Error
						errorTemp.errorMessage = DATABASEERRORTMESSAGE;
						errorTemp.errorNumber = DATABASEERRORNUMBER;
					}
				}
			}
			else
			{
				// Fill error in response
				errorTemp.errorMessage = DEVICEEXISTMESSAGE;
				errorTemp.errorNumber = DEVICEEXISTNUMBER;
			}
			responseErrorInfo.push_back(errorTemp);
		}
	}
	else
	{
		// Fill error in response
		errorTemp.errorMessage = USERAUTHENTICATIONMESSAGE;
		errorTemp.errorNumber = USERAUTHENTICATIONNUMBER;
		responseErrorInfo.push_back(errorTemp);
	}

	return SOAP_OK;
}

xsd__int ns__DeleteDevices(soap* soap, ns__User requestUserInfo, std::vector<ns__Device> requestDeviceList, ns__ErrorCode &responseErrorInfo)
{
	return SOAP_OK;
}

xsd__int ns__Login(soap* soap, ns__User requestUserInfo, ns__User &responseUserInfo)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;

	// Initialize
	soap_default_tm(soap, &(requestUserInfo.userSubmitDateTime));
	responseUserInfo = requestUserInfo;

	// User Authentication checking
	responseUserInfo.userId = erObject.AuthenticateUser(responseUserInfo);
	if(responseUserInfo.userId < 1)
	{
		// Fill error in response
		responseUserInfo.userError.errorMessage = USERAUTHENTICATIONMESSAGE;
		responseUserInfo.userError.errorNumber = USERAUTHENTICATIONNUMBER;
	}

	return SOAP_OK;
}

xsd__int ns__GetUserDeviceList(soap* soap, ns__User requestUserInfo, ns__UserDevice &responseUserDevicesInfo)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;
	soap_default_tm(soap, &responseUserDevicesInfo.userDeviceUser.userSubmitDateTime);

	// User Authentication checking
	requestUserInfo.userId = erObject.AuthenticateUser(requestUserInfo);
	if(requestUserInfo.userId > 0)
	{
		if(requestUserInfo.userType == ns__UserType::Admin)
		{
			if(erObject.GetAllDeviceList(responseUserDevicesInfo.userDeviceDevices.userDevices) != 1)
			{
				// No device found error
				responseUserDevicesInfo.userDeviceError.errorMessage = NODEVICEFOUNDMESSAGE;
				responseUserDevicesInfo.userDeviceError.errorNumber = NODEVICEFOUNDNUMBER;
			}
			else
			{
				for(int i = 0; i < responseUserDevicesInfo.userDeviceDevices.userDevices.size(); i++)
				{
					erObject.DeviceInfoToBase64(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice);

					soap_default_tm(soap, &(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice.deviceDateTime));
				}
			}
		}
		else
		{
			if(erObject.GetUserDeviceList(requestUserInfo, responseUserDevicesInfo.userDeviceDevices.userDevices) != 1)
			{
				// No device found error
				responseUserDevicesInfo.userDeviceError.errorMessage = NODEVICEFOUNDMESSAGE;
				responseUserDevicesInfo.userDeviceError.errorNumber = NODEVICEFOUNDNUMBER;
			}
			else
			{
				for(int i = 0; i < responseUserDevicesInfo.userDeviceDevices.userDevices.size(); i++)
				{
					erObject.DeviceInfoToBase64(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice);

					soap_default_tm(soap, &(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice.deviceDateTime));
				}
			}
		}
	}
	else
	{
		// Fill error in response
		responseUserDevicesInfo.userDeviceError.errorMessage = USERAUTHENTICATIONMESSAGE;
		responseUserDevicesInfo.userDeviceError.errorNumber = USERAUTHENTICATIONNUMBER;
	}

	return SOAP_OK;
}

xsd__int ns__GetAllDeviceList(soap* soap, ns__User requestUserInfo, ns__UserDevice &responseUserDevicesInfo)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;
	soap_default_tm(soap, &(responseUserDevicesInfo.userDeviceUser.userSubmitDateTime));

	// User Authentication checking
	requestUserInfo.userId = erObject.AuthenticateUser(requestUserInfo);
	if(requestUserInfo.userId > 0)
	{
		if(requestUserInfo.userType == ns__UserType::Admin)
		{
			if(erObject.GetAllDeviceList(responseUserDevicesInfo.userDeviceDevices.userDevices) != 1)
			{
				// No device found error
				responseUserDevicesInfo.userDeviceError.errorMessage = NODEVICEFOUNDMESSAGE;
				responseUserDevicesInfo.userDeviceError.errorNumber = NODEVICEFOUNDNUMBER;
			}
			else
			{
				for(int i = 0; i < responseUserDevicesInfo.userDeviceDevices.userDevices.size(); i++)
				{
					erObject.DeviceInfoToBase64(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice);

					soap_default_tm(soap, &(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice.deviceDateTime));
				}
			}
		}
		else
		{
			if(erObject.GetUserDeviceList(requestUserInfo, responseUserDevicesInfo.userDeviceDevices.userDevices) != 1)
			{
				// No device found error
				responseUserDevicesInfo.userDeviceError.errorMessage = NODEVICEFOUNDMESSAGE;
				responseUserDevicesInfo.userDeviceError.errorNumber = NODEVICEFOUNDNUMBER;
			}
			else
			{
				for(int i = 0; i < responseUserDevicesInfo.userDeviceDevices.userDevices.size(); i++)
				{
					erObject.DeviceInfoToBase64(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice);

					soap_default_tm(soap, &(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice.deviceDateTime));
				}
			}
		}
	}
	else
	{
		// Fill error in response
		responseUserDevicesInfo.userDeviceError.errorMessage = USERAUTHENTICATIONMESSAGE;
		responseUserDevicesInfo.userDeviceError.errorNumber = USERAUTHENTICATIONNUMBER;
	}

	return SOAP_OK;
}



xsd__int ns__GetUsersList(soap* soap, ns__User requestUserInfo, ns__UsersList &responseUsersList)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;

	responseUsersList.usersListUsers.clear();

	// User Authentication checking
	requestUserInfo.userId = erObject.AuthenticateUser(requestUserInfo);
	if(requestUserInfo.userId > 0)
	{
		if(requestUserInfo.userType == ns__UserType::Admin)
		{
			if(erObject.GetUsersList(responseUsersList.usersListUsers) > 0)
			{
				for(int i = 0; i < responseUsersList.usersListUsers.size(); i++)
				{
					soap_default_tm(soap, &(responseUsersList.usersListUsers[i].userSubmitDateTime));
				}
			}
			else
			{
				// No device found error
				responseUsersList.usersListError.errorMessage = NOUSERFOUNDMESSAGE;
				responseUsersList.usersListError.errorNumber = NOUSERFOUNDNUMBER;
			}
		}
		else
		{
			responseUsersList.usersListError.errorMessage = USERPERMISSIONMESSAGE;
			responseUsersList.usersListError.errorNumber = USERPERMISSIONNUMBER;
		}
	}
	else
	{
		// Fill error in response
		responseUsersList.usersListError.errorMessage = USERAUTHENTICATIONMESSAGE;
		responseUsersList.usersListError.errorNumber = USERAUTHENTICATIONNUMBER;
	}

	return SOAP_OK;
}

xsd__int ns__AddUser(soap* soap, ns__User requestUserInfo, ns__User requestNewUserInfo, ns__ErrorCode &responseErrorInfo)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;

	responseErrorInfo.errorMessage = "";
	responseErrorInfo.errorNumber = 0;

	// User Authentication checking
	requestUserInfo.userId = erObject.AuthenticateUser(requestUserInfo);
	if(requestUserInfo.userId > 0)
	{
		if(requestUserInfo.userType == ns__UserType::Admin)
		{
			if(erObject.AddUser(requestNewUserInfo) > 0)
			{
			}
			else
			{
				// No device found error
				responseErrorInfo.errorMessage = NOUSERFOUNDMESSAGE;
				responseErrorInfo.errorNumber = NOUSERFOUNDNUMBER;
			}
		}
		else
		{
			responseErrorInfo.errorMessage = USERPERMISSIONMESSAGE;
			responseErrorInfo.errorNumber = USERPERMISSIONNUMBER;
		}
	}
	else
	{
		// Fill error in response
		responseErrorInfo.errorMessage = USERAUTHENTICATIONMESSAGE;
		responseErrorInfo.errorNumber = USERAUTHENTICATIONNUMBER;
	}

	return SOAP_OK;
}

xsd__int ns__UpdateUser(soap* soap, ns__User requestUserInfo, ns__User requestNewUserInfo, ns__ErrorCode &responseErrorInfo)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;

	responseErrorInfo.errorMessage = "";
	responseErrorInfo.errorNumber = 0;

	// User Authentication checking
	requestUserInfo.userId = erObject.AuthenticateUser(requestUserInfo);
	if(requestUserInfo.userId > 0)
	{
		if(requestUserInfo.userType == ns__UserType::Admin)
		{
			if(erObject.UpdateUser(requestNewUserInfo) > 0)
			{
			}
			else
			{
				// No user found error
				responseErrorInfo.errorMessage = NOUSERFOUNDMESSAGE;
				responseErrorInfo.errorNumber = NOUSERFOUNDNUMBER;
			}
		}
		else
		{
			responseErrorInfo.errorMessage = USERPERMISSIONMESSAGE;
			responseErrorInfo.errorNumber = USERPERMISSIONNUMBER;
		}
	}
	else
	{
		// Fill error in response
		responseErrorInfo.errorMessage = USERAUTHENTICATIONMESSAGE;
		responseErrorInfo.errorNumber = USERAUTHENTICATIONNUMBER;
	}

	return SOAP_OK;
}

xsd__int ns__DeleteUser(soap* soap, ns__User requestUserInfo, ns__User requestDeleteUserInfo, ns__ErrorCode &responseErrorInfo)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;

	responseErrorInfo.errorMessage = "";
	responseErrorInfo.errorNumber = 0;

	// User Authentication checking
	requestUserInfo.userId = erObject.AuthenticateUser(requestUserInfo);
	if(requestUserInfo.userId > 0)
	{
		if(requestUserInfo.userType == ns__UserType::Admin)
		{
			if(erObject.DeleteUser(requestDeleteUserInfo) > 0)
			{
			}
			else
			{
				// No device found error
				responseErrorInfo.errorMessage = NOUSERFOUNDMESSAGE;
				responseErrorInfo.errorNumber = NOUSERFOUNDNUMBER;
			}
		}
		else
		{
			responseErrorInfo.errorMessage = USERPERMISSIONMESSAGE;
			responseErrorInfo.errorNumber = USERPERMISSIONNUMBER;
		}
	}
	else
	{
		// Fill error in response
		responseErrorInfo.errorMessage = USERAUTHENTICATIONMESSAGE;
		responseErrorInfo.errorNumber = USERAUTHENTICATIONNUMBER;
	}

	return SOAP_OK;
}

xsd__int ns__AssignDeviceToUser(soap* soap, ns__User requestUserInfo, ns__UserDevice requestUserDeviceInfo, ns__ErrorCode &responseErrorInfo)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;

	responseErrorInfo.errorMessage = "";
	responseErrorInfo.errorNumber = 0;

	// User Authentication checking
	requestUserInfo.userId = erObject.AuthenticateUser(requestUserInfo);
	if(requestUserInfo.userId > 0)
	{
		if(requestUserInfo.userType == ns__UserType::Admin)
		{
			if(erObject.AssignDeviceToUser(requestUserDeviceInfo) > 0)
			{
			}
			else
			{
				// No device found error
				responseErrorInfo.errorMessage = NOUSERFOUNDMESSAGE;
				responseErrorInfo.errorNumber = NOUSERFOUNDNUMBER;
			}
		}
		else
		{
			responseErrorInfo.errorMessage = USERPERMISSIONMESSAGE;
			responseErrorInfo.errorNumber = USERPERMISSIONNUMBER;
		}
	}
	else
	{
		// Fill error in response
		responseErrorInfo.errorMessage = USERAUTHENTICATIONMESSAGE;
		responseErrorInfo.errorNumber = USERAUTHENTICATIONNUMBER;
	}

	return SOAP_OK;
}

xsd__int ns__DeleteDeviceFromUserList(soap* soap, ns__User requestUserInfo, ns__UserDevice requestUserDeviceInfo, ns__ErrorCode &responseErrorInfo)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;

	responseErrorInfo.errorMessage = "";
	responseErrorInfo.errorNumber = 0;

	// User Authentication checking
	requestUserInfo.userId = erObject.AuthenticateUser(requestUserInfo);
	if(requestUserInfo.userId > 0)
	{
		if(requestUserInfo.userType == ns__UserType::Admin)
		{
			if(erObject.DeleteDeviceFromUser(requestUserDeviceInfo) > 0)
			{
			}
			else
			{
				// No device found error
				responseErrorInfo.errorMessage = NOUSERFOUNDMESSAGE;
				responseErrorInfo.errorNumber = NOUSERFOUNDNUMBER;
			}
		}
		else
		{
			responseErrorInfo.errorMessage = USERPERMISSIONMESSAGE;
			responseErrorInfo.errorNumber = USERPERMISSIONNUMBER;
		}
	}
	else
	{
		// Fill error in response
		responseErrorInfo.errorMessage = USERAUTHENTICATIONMESSAGE;
		responseErrorInfo.errorNumber = USERAUTHENTICATIONNUMBER;
	}

	return SOAP_OK;
}

xsd__int ns__GetUserDevices(soap* soap, ns__User requestUserInfo, ns__User requestSelectedUserInfo, ns__UserDevice &responseUserDevicesInfo)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;

	soap_default_tm(soap, &(requestSelectedUserInfo.userSubmitDateTime));
	responseUserDevicesInfo.userDeviceUser = requestSelectedUserInfo;

	// User Authentication checking
	requestUserInfo.userId = erObject.AuthenticateUser(requestUserInfo);
	if(requestUserInfo.userId > 0)
	{
		if(requestUserInfo.userType == ns__UserType::Admin)
		{
			responseUserDevicesInfo.userDeviceUser = requestSelectedUserInfo;

			if(erObject.GetUserDeviceList(requestSelectedUserInfo, responseUserDevicesInfo.userDeviceDevices.userDevices) != 1)
			{
				// No device found error
				responseUserDevicesInfo.userDeviceError.errorMessage = NODEVICEFOUNDMESSAGE;
				responseUserDevicesInfo.userDeviceError.errorNumber = NODEVICEFOUNDNUMBER;
			}
			else
			{
				for(int i = 0; i < responseUserDevicesInfo.userDeviceDevices.userDevices.size(); i++)
				{
					soap_default_tm(soap, &(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice.deviceDateTime));

					// Get other device information from database
					if(erObject.GetDeviceSetting(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice) == 1)
					{
						erObject.DeviceInfoToBase64(responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice);
					}
					else
					{
						// Database Error
						responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice.deviceError.errorMessage = DATABASEERRORTMESSAGE;
						responseUserDevicesInfo.userDeviceDevices.userDevices[i].userDeviceDevice.deviceError.errorNumber = DATABASEERRORNUMBER;
					}
				}
			}
		}
		else
		{
		}
	}
	else
	{
		// Fill error in response
		responseUserDevicesInfo.userDeviceError.errorMessage = USERAUTHENTICATIONMESSAGE;
		responseUserDevicesInfo.userDeviceError.errorNumber = USERAUTHENTICATIONNUMBER;
	}

	return SOAP_OK;
}



xsd__int ns__SaveActivityLog(soap* soap, ns__User requestUserInfo, ns__ActivityLog requestActivityLogInfo, ns__ErrorCode &responseErrorInfo)
{
	return SOAP_OK;
}

xsd__int ns__GetActivityLog(soap* soap, ns__User requestUserInfo, ns__Report requestReportInfo, std::vector<ns__ActivityLog> &responseActivityLogInfo)
{
	return SOAP_OK;
}

xsd__int ns__SaveUserLog(soap* soap, ns__User requestUserInfo, ns__UserLog requestActivityLogInfo, ns__ErrorCode &responseErrorInfo)
{
	return SOAP_OK;
}

xsd__int ns__GetUserLog(soap* soap, ns__User requestUserInfo, ns__Report requestReportInfo, std::vector<ns__UserLog> &responseActivityLogInfo)
{
	return SOAP_OK;
}

xsd__int ns__SaveDeviceStatusLog(soap* soap, ns__User requestUserInfo, ns__DeviceStatusLog requestStatusLogInfo, ns__ErrorCode &responseErrorInfo)
{
	return SOAP_OK;
}

xsd__int ns__GetDeviceStatusLog(soap* soap, ns__User requestUserInfo, ns__DeviceStatusInfo requestStatusLogInfo, ns__DeviceStatusLogList &responseStatusLogList)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;

	soap_default_tm(soap, &(requestUserInfo.userSubmitDateTime));

	// User Authentication checking
	requestUserInfo.userId = erObject.AuthenticateUser(requestUserInfo);
	if(requestUserInfo.userId > 0)
	{
		// Check user should be Admin or Control (no Monitor)
		if(requestUserInfo.userType == ns__UserType::Admin || requestUserInfo.userType == Control)
		{
			if(erObject.GetDeviceStatusLog(requestStatusLogInfo, responseStatusLogList) > 0)
			{
				// Base64 encoding data
				Base64 b64;
				for(int i = 0; i < responseStatusLogList.statusLogListStatus.size(); i++)
				{
					erObject.DeviceInfoToBase64(responseStatusLogList.statusLogListStatus[i].statusLogDevice);
					soap_default_tm(soap, &(responseStatusLogList.statusLogListStatus[i].statusLogDevice.deviceDateTime));
					soap_default_tm(soap, &(responseStatusLogList.statusLogListStatus[i].statusLogStartEndDateTime.reportEndDate));
				}
			}
			else
			{
				// Fill error in response
				responseStatusLogList.statusLogListError.errorMessage = NORECORDFOUNDMESSAGE;
				responseStatusLogList.statusLogListError.errorNumber = NORECORDFOUNDNUMBER;
			}
		}
		else
		{
			// Fill error in response
			responseStatusLogList.statusLogListError.errorMessage = USERAUTHENTICATIONMESSAGE;
			responseStatusLogList.statusLogListError.errorNumber = USERAUTHENTICATIONNUMBER;
		}
	}
	else
	{
		// Fill error in response
		responseStatusLogList.statusLogListError.errorMessage = USERAUTHENTICATIONMESSAGE;
		responseStatusLogList.statusLogListError.errorNumber = USERAUTHENTICATIONNUMBER;
	}
	soap_default_tm(soap, &(requestStatusLogInfo.deviceStatusInfoStartEndDateTime.reportStartDate));
	soap_default_tm(soap, &(requestStatusLogInfo.deviceStatusInfoStartEndDateTime.reportEndDate));

	return SOAP_OK;
}



xsd__int ns__GetServerOptions(soap* soap, ns__User requestUserInfo, ns__ServerOptions &serverOptions)
{
	return SOAP_OK;
}

xsd__int ns__SetServerOptions(soap* soap, ns__User requestUserInfo, ns__ServerOptions serverOptions, ns__ErrorCode &responseErrorInfo)
{
	return SOAP_OK;
}



xsd__int ns__GetCityAndLocations(soap* soap, ns__User requestUserInfo, ns__AllCityLocatoins &responseCityLocationInfo)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;

	// User Authentication checking
	requestUserInfo.userId = erObject.AuthenticateUser(requestUserInfo);
	if(requestUserInfo.userId > 0)
	{
		if(requestUserInfo.userType == ns__UserType::Admin || requestUserInfo.userType == Control)
		{
			if(erObject.GetDeviceCityLocationList(responseCityLocationInfo) < 1)
			{
				responseCityLocationInfo.allCityLocatoinsError.errorMessage = NOCITYLOCATIONMESSAGE;
				responseCityLocationInfo.allCityLocatoinsError.errorNumber = NOCITYLOCATIONNUMBER;
			}
			else
			{
				if(responseCityLocationInfo.allCityLocatoins.size() > 0)
				{
					Base64 b64;
					for(int i = 0; i < responseCityLocationInfo.allCityLocatoins.size(); i++)
					{
						// Encode to base 64 for unicode string
						responseCityLocationInfo.allCityLocatoins[i].cityLocationCityName = b64.Encode(responseCityLocationInfo.allCityLocatoins[i].cityLocationCityName);
						if(responseCityLocationInfo.allCityLocatoins[i].cityLocationLocationsName.size() > 0)
						{
							for(int k = 0; k < responseCityLocationInfo.allCityLocatoins[i].cityLocationLocationsName.size(); k++)
							{
								responseCityLocationInfo.allCityLocatoins[i].cityLocationLocationsName[k] = b64.Encode(responseCityLocationInfo.allCityLocatoins[i].cityLocationLocationsName[k]);
							}
						}
					}
				}
				else
				{
					responseCityLocationInfo.allCityLocatoinsError.errorMessage = NOITEMFOUNDMESSAGE;
					responseCityLocationInfo.allCityLocatoinsError.errorNumber = NOITEMFOUNDNUMBER;
				}
			}
		}
		else
		{
			responseCityLocationInfo.allCityLocatoinsError.errorMessage = DEVICEPERMISSIONMESSAGE;
			responseCityLocationInfo.allCityLocatoinsError.errorNumber = DEVICEPERMISSIONNUMBER;
		}
	}
	else
	{
		// Fill error in response
		responseCityLocationInfo.allCityLocatoinsError.errorMessage = USERAUTHENTICATIONMESSAGE;
		responseCityLocationInfo.allCityLocatoinsError.errorNumber = USERAUTHENTICATIONNUMBER;
	}

	return SOAP_OK;
}

xsd__int ns__ChangeCityName(soap* soap, ns__User requestUserInfo, xsd__string oldCityName, xsd__string newCityName, ns__ErrorCode &responseErrorInfo)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;

	// User Authentication checking
	requestUserInfo.userId = erObject.AuthenticateUser(requestUserInfo);
	if(requestUserInfo.userId > 0)
	{
		if(requestUserInfo.userType == ns__UserType::Admin || requestUserInfo.userType == Control)
		{
			Base64 b64;
			if(erObject.ChangeCityName(b64.Decode(oldCityName), b64.Decode(newCityName)) < 1)
			{
				responseErrorInfo.errorMessage = UPDATECITYNAME;
				responseErrorInfo.errorNumber = UPDATECITYNUMBER;
			}
		}
		else
		{
			responseErrorInfo.errorMessage = DEVICEPERMISSIONMESSAGE;
			responseErrorInfo.errorNumber = DEVICEPERMISSIONNUMBER;
		}
	}
	else
	{
		// Fill error in response
		responseErrorInfo.errorMessage = USERAUTHENTICATIONMESSAGE;
		responseErrorInfo.errorNumber = USERAUTHENTICATIONNUMBER;
	}

	return SOAP_OK;
}

xsd__int ns__ChangeLocationName(soap* soap, ns__User requestUserInfo, xsd__string oldCityName, xsd__string newCityName, xsd__string oldLocationName, xsd__string newLocationName, ns__ErrorCode &responseErrorInfo)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;

	// User Authentication checking
	requestUserInfo.userId = erObject.AuthenticateUser(requestUserInfo);
	if(requestUserInfo.userId > 0)
	{
		if(requestUserInfo.userType == ns__UserType::Admin || requestUserInfo.userType == Control)
		{
			Base64 b64;
			if(erObject.ChangeLocationName(b64.Decode(oldCityName), b64.Decode(newCityName), b64.Decode(oldLocationName), b64.Decode(newLocationName)) < 1)
			{
				responseErrorInfo.errorMessage = UPDATELOCATIONNAME;
				responseErrorInfo.errorNumber = UPDATELOCATIONNUMBER;
			}
		}
		else
		{
			responseErrorInfo.errorMessage = DEVICEPERMISSIONMESSAGE;
			responseErrorInfo.errorNumber = DEVICEPERMISSIONNUMBER;
		}
	}
	else
	{
		// Fill error in response
		responseErrorInfo.errorMessage = USERAUTHENTICATIONMESSAGE;
		responseErrorInfo.errorNumber = USERAUTHENTICATIONNUMBER;
	}

	return SOAP_OK;
}

xsd__int ns__ChangeDeviceName(soap* soap, ns__User requestUserInfo, xsd__int deviceSerialNumber, xsd__string newCityName, xsd__string newLocationName, xsd__string newDeviceName, ns__ErrorCode &responseErrorInfo)
{
	// Check received data format and values

	// Create an object from ExecuteReauest class
	ExecuteRequest erObject;

	// User Authentication checking
	requestUserInfo.userId = erObject.AuthenticateUser(requestUserInfo);
	if(requestUserInfo.userId > 0)
	{
		if(requestUserInfo.userType == ns__UserType::Admin || requestUserInfo.userType == Control)
		{
			Base64 b64;
			if(erObject.ChangeDeviceName(deviceSerialNumber, b64.Decode(newCityName), b64.Decode(newLocationName), b64.Decode(newDeviceName)) < 1)
			{
				responseErrorInfo.errorMessage = UPDATEDEVICENAMEMESSAGE;
				responseErrorInfo.errorNumber = UPDATEDEVICENAMENUMBER;
			}
		}
		else
		{
			responseErrorInfo.errorMessage = DEVICEPERMISSIONMESSAGE;
			responseErrorInfo.errorNumber = DEVICEPERMISSIONNUMBER;
		}
	}
	else
	{
		// Fill error in response
		responseErrorInfo.errorMessage = USERAUTHENTICATIONMESSAGE;
		responseErrorInfo.errorNumber = USERAUTHENTICATIONNUMBER;
	}

	return SOAP_OK;
}

xsd__int ns__TestWorld(soap* soap, xsd__dateTime requestUserInfo, xsd__dateTime &responseErrorInfo)
{
	soap_default_tm(soap, &responseErrorInfo);
	soap_default_tm(soap, &requestUserInfo);
	//responseErrorInfo.tm_year = 116;
	return SOAP_OK;
}