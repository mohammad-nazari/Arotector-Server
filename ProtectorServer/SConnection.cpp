#include "SConnection.h"

extern std::vector<ns__Device> deviceInfoList;
extern std::map<int, DeviceMap> deviceMap;

SConnection::SConnection()
{
	this->ListenSocket = INVALID_SOCKET;

	/* Initialize the critical section before entering multi-threaded context. */
	InitializeCriticalSection(&(this->csDeviceList));
}

SConnection::~SConnection(void)
{
	/* Release system object when all finished -- usually at the end of the cleanup code */
	DeleteCriticalSection(&(this->csDeviceList));
}

int SConnection::CreateSocket()
{
	WSADATA wsaData;
	int iResult;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// Initialize WinSock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		//printf("WSAStartup failed with Error :  %d\n", iResult);
		return 0;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		//printf("getaddrinfo failed with Error :  %d\n", iResult);
		WSACleanup();
		return 0;
	}

	// Create a SOCKET for connecting to server
	this->ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (this->ListenSocket == INVALID_SOCKET)
	{
		//printf("socket failed with Error :  %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 0;
	}

	// Setup the TCP listening socket
	iResult = bind(this->ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		//printf("bind failed with Error :  %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(this->ListenSocket);
		WSACleanup();
		return 0;
	}

	iResult = listen(this->ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		//printf("listen failed with Error :  %d\n", WSAGetLastError());
		closesocket(this->ListenSocket);
		WSACleanup();
		return 0;
	}

	// Set time out
	int nTimeout = CONNECTIONTIMEOUT;
	iResult = setsockopt(this->ListenSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&nTimeout, sizeof(int));
	if (iResult == SOCKET_ERROR)
	{
		//printf("listen failed with Error :  %d\n", WSAGetLastError());
		closesocket(this->ListenSocket);
		WSACleanup();
		return 0;
	}

	iResult = setsockopt(this->ListenSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&nTimeout, sizeof(int));
	if (iResult == SOCKET_ERROR)
	{
		//printf("listen failed with Error :  %d\n", WSAGetLastError());
		closesocket(this->ListenSocket);
		WSACleanup();
		return 0;
	}

	int buffsize = 65000;
	iResult = setsockopt(this->ListenSocket, SOL_SOCKET, SO_RCVBUF, (char *)&buffsize, sizeof(buffsize));
	if (iResult == SOCKET_ERROR)
	{
		//printf("listen failed with Error :  %d\n", WSAGetLastError());
		closesocket(this->ListenSocket);
		WSACleanup();
		return 0;
	}

	/*//std::cout << " On " << result->ai_addr << " : " << DEFAULT_PORT << std::endl;*/

	freeaddrinfo(result);

	return 1;
}

int SConnection::CloseSocket()
{
	/* Clean up */
	closesocket(this->ListenSocket);
	WSACleanup();

	return 1;
}

int SConnection::HandleDevice(ns__Device Input)
{
	ns__Device deviceInfo;
	DeviceMap deviceMapInfo;

	/* send(), recv(), closesocket() */
	Base64 base64Object;
	SyntaxChecker parserObject;

	std::string DeviceStatusString = "", DeviceStatusStringBackup = "";

	Input.deviceSerialNumber = this->IsDevice((SOCKET)Input.deviceSocket);

	// It is a device
	if (Input.deviceSerialNumber > 0)
	{
		int bytesReceivedFromClientMsg = 1;
		char buf[MAX_DATA] = { '\0' };

		std::string strMessage = std::to_string(Input.deviceSerialNumber) + ">STT$";

		// Get Device Status
		int bytes_sent = this->WriteData((SOCKET)Input.deviceSocket, strMessage.c_str());
		if (bytes_sent == SOCKET_ERROR)
		{
			//std::cerr << "Error :  " << WSAGetLastError() << std::endl;
			return -1;
		}

		bytesReceivedFromClientMsg = this->ReadData((SOCKET)Input.deviceSocket, strMessage);
		if (bytesReceivedFromClientMsg == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAECONNRESET)
			{
				//std::cerr << "recv() Error :  " << WSAGetLastError() << std::endl;
			}
			closesocket(Input.deviceSocket);
		}
		else
		{
			if (bytesReceivedFromClientMsg != 0)
			{
				if (parserObject.CheckSTTSyntax(strMessage))
				{
					deviceInfo = parserObject.get_deviceInfo();
					deviceInfo.deviceSocket = Input.deviceSocket;
					deviceInfo.deviceIP = Input.deviceIP;
					deviceInfo.devicePort = Input.devicePort;

					//std::cout << "Serial number: (" << deviceInfo.deviceSerialNumber << ") IP Value: (" << deviceInfo.deviceIP.ipFirst << deviceInfo.deviceIP.ipSecond << deviceInfo.deviceIP.ipThird << deviceInfo.deviceIP.ipFourth << ") Port number: (" << deviceInfo.devicePort << ") Socket number: (" << deviceInfo.deviceSocket << ")" << std::endl;

					/*
					* Check device is in list or no
					* It is old device
					* Just add to list of sockets
					*/
					int existDevice = this->AddUpdateDeletSearchDeviceInList(deviceInfo, 4, deviceMapInfo);

					// Device be added in the past and a thread exist for that currently
					if (existDevice == 1)
					{
						// Just update device info in database
						if (this->UpdateDeviceRow(deviceInfo) > 0)
						{
							// Add to device list
							existDevice = this->AddUpdateDeletSearchDeviceInList(deviceInfo, 1, deviceMapInfo);
						}
						else
						{
							//std::cerr << "\nError :  " << WSAGetLastError() << std::endl;
							closesocket(deviceInfo.deviceSocket);
							return -1;
						}
					}
					else if (existDevice == 0)
					{
						// Generate Row in Database
						if (this->CreateDeviceRow(deviceInfo) > 0)
						{
							// Add to device list
							existDevice = this->AddUpdateDeletSearchDeviceInList(deviceInfo, 1, deviceMapInfo);
						}
						else
						{
							std::cerr << "\nError :  Can not create device row in database" << std::endl;
							closesocket(deviceInfo.deviceSocket);
							return -1;
						}
					}

					/*
					* If no thread exist for this device
					* this thread assign to this device
					*/
					//if (existDevice != -1)
					{

						// Analyze and make device status
						this->InitiateDeviceStatus(deviceInfo, DeviceStatusString);

						if (DeviceStatusString != DeviceStatusStringBackup)
						{
							this->SaveDevieStatus(deviceInfo, DeviceStatusString);
							DeviceStatusStringBackup = DeviceStatusString;
						}

						int tryCounter = 0;	// try send command and get data 5 time, after that connection closed
						/*
						* If device exist in list of devices
						* means a thread exist (created in past)
						* for this device and now not need
						* generate new thread
						* so device Number is -1
						* and we does not entered to infinite while
						*/
						while (tryCounter < 5)
						{
							// Check CNF commands and do them first
							AddUpdateDeletSearchDeviceInList(deviceInfo, 7, deviceMapInfo);

							if (deviceMapInfo.CommandString != "")
							{
								bytesReceivedFromClientMsg = 1;
								buf[0] = '\0';
								strMessage = deviceMapInfo.CommandString;

								bytes_sent = WriteData(deviceInfo.deviceSocket, strMessage.c_str());
								//Sleep(SLEEPTIMEVALUE); // Wait device refresh yourself

								bytesReceivedFromClientMsg = ReadData(deviceInfo.deviceSocket, strMessage);

								if (bytesReceivedFromClientMsg != SOCKET_ERROR)
								{
									if (bytesReceivedFromClientMsg != 0)
									{
										deviceMapInfo.CommandResurl = strMessage;
										// Update result value
										AddUpdateDeletSearchDeviceInList(deviceInfo, 6, deviceMapInfo);
									}
									else
									{
										deviceMapInfo.CommandResurl = std::to_string(deviceInfo.deviceSerialNumber) + ">NOK$";
										AddUpdateDeletSearchDeviceInList(deviceInfo, 6, deviceMapInfo);
									}
								}
								else
								{
									deviceMapInfo.CommandResurl = std::to_string(deviceInfo.deviceSerialNumber) + ">NOK$";
									AddUpdateDeletSearchDeviceInList(deviceInfo, 6, deviceMapInfo);
								}
							}

							/*
							* Check device is in database or no
							* It is old device
							* Just add to list of sockets
							*/
							existDevice = this->AddUpdateDeletSearchDeviceInList(deviceInfo, 4, deviceMapInfo);

							/*
							* Every 1 second we get a status
							* from device and save it
							* for send to user for every STT command
							*/

							if (existDevice == 1)
							{
								bytesReceivedFromClientMsg = 1;
								buf[0] = '\0';
								strMessage = std::to_string(deviceInfo.deviceSerialNumber) + ">STT$";

								// Update Status
								bytes_sent = this->WriteData(deviceInfo.deviceSocket, strMessage.c_str());

								bytesReceivedFromClientMsg = this->ReadData(deviceInfo.deviceSocket, strMessage);
								//std::cout << "\n" << strMessage << std::endl;

								if (bytesReceivedFromClientMsg != SOCKET_ERROR)
								{
									if (bytesReceivedFromClientMsg != 0)
									{
										if (parserObject.CheckSTTSyntax(strMessage))
										{
											deviceInfo = parserObject.get_deviceInfo();
											deviceInfo.deviceSocket = Input.deviceSocket;
											deviceInfo.deviceIP = Input.deviceIP;
											deviceInfo.devicePort = Input.devicePort;

											this->AddUpdateDeletSearchDeviceInList(deviceInfo, 2, deviceMapInfo);

											// Analyze and make device status
											this->InitiateDeviceStatus(deviceInfo, DeviceStatusString);

											if (DeviceStatusString != DeviceStatusStringBackup)
											{
												this->SaveDevieStatus(deviceInfo, DeviceStatusString);
												DeviceStatusStringBackup = DeviceStatusString;
											}
										}
										else
										{
											deviceInfo.deviceError.errorMessage = std::to_string(deviceInfo.deviceSerialNumber) + ">NOK$";
											AddUpdateDeletSearchDeviceInList(deviceInfo, 2, deviceMapInfo);
											tryCounter++;	// Add try
										}
									}
									else
									{
										deviceInfo.deviceError.errorMessage = std::to_string(deviceInfo.deviceSerialNumber) + ">NOK$";
										AddUpdateDeletSearchDeviceInList(deviceInfo, 2, deviceMapInfo);
										tryCounter++;	// Add try
									}
								}
								else
								{
									deviceInfo.deviceError.errorMessage = std::to_string(deviceInfo.deviceSerialNumber) + ">NOK$";
									AddUpdateDeletSearchDeviceInList(deviceInfo, 2, deviceMapInfo);
									tryCounter = 5;	// Add try
								}
							}
							else
							{
								/* Device is out of service
								* and or deleted from system physically
								* So close socket
								* and end the thread
								*/
								std::cout << "Device number " << deviceInfo.deviceSerialNumber << " is out is out of and or deleted from system physically" << std::endl;
								tryCounter = 5;	// Add try
								break; // While
							}
						} // End while
						/*
						*	Thread end
						*	and device should be deleted from list
						*/
						if (this->AddUpdateDeletSearchDeviceInList(deviceInfo, 3, deviceMapInfo))
						{
							std::cout << "\nDevice (" << deviceInfo.deviceSerialNumber << ") Deleted From List" << std::endl;
						}
						else
						{
							std::cout << "\nError in deleting device (" << deviceInfo.deviceSerialNumber << ") from list" << std::endl;
						}
					}
				}
				else
				{
					std::cerr << "Error :  " << WSAGetLastError() << std::endl;
				}
			}
			else
			{
				std::cout << "Connection Closed" << std::endl;
			}
		}
	}
	else
	{
		// Unknown connection
		std::cout << "Unknown connection" << std::endl;
	}
	try
	{
		closesocket(deviceInfo.deviceSocket);
		std::cout << "Socket (" << deviceInfo.deviceSocket << ") Closed successfully" << std::endl;
	}
	catch (std::exception e)
	{
		std::cout << "Socket (" << deviceInfo.deviceSocket << ") Closed unsuccessfully" << std::endl;
	}
	_endthread();

	return 1;
}

int SConnection::IsDevice(SOCKET pParam)
{
	Base64 coder;
	int bytes_sent = 0;
	//std::string strMessage = Encode("YOU$");
	std::string strMessage = "YOU$";

	bytes_sent = this->WriteData(pParam, strMessage.c_str());
	if (bytes_sent == SOCKET_ERROR)
	{
		//std::cerr << "Error :  " << WSAGetLastError( ) << std::endl;
		return -1;
	}
	//std::cout << SERVERMESSAGE2NEW << strMessage << std::endl;

	int bytesReceivedFromClientMsg = this->ReadData(pParam, strMessage);
	if (bytesReceivedFromClientMsg == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAECONNRESET)
		{
			//std::cerr << "recv() Error :  " << WSAGetLastError() << std::endl;
			return -1;
		}
	}
	if (bytesReceivedFromClientMsg != 0)
	{
		//std::cout << NEWMESSAE2USER << strMessage << std::endl;

		Splitter sp(strMessage, ">");

		if (sp.size() == 2 && this->toolsObject.toUpper(sp[1]) == "DEV$")
		{
			return atoi(sp[0].c_str());
		}
		else
		{
			return -1;
		}
	}
	else
	{
		// Connection closed
		return -1;
	}
}

int SConnection::WriteData(SOCKET Socket, const std::string strMessage)
{
	int bytes_sent;
	int total_bytes_sent = 0;

	// Base64
	//std::string MessageData = Encode(strMessage);
	std::string MessageData = strMessage;

	bytes_sent = send(Socket, MessageData.c_str(), (int)MessageData.size(), 0);

	return bytes_sent;
}

int SConnection::ReadData(SOCKET Socket, std::string& strMessage)
{
	char buf[MAX_DATA] = { '\0' };
	int bytesReceivedFromClientMsg = 0;
	int bytesReceivedFromClientMsgSum = 1;
	strMessage.clear();

	Sleep(SLEEPTIMEVALUE); // Wait to send next STT command
	do
	{
		memset(buf, 0, MAX_DATA);  //clear the variable
		bytesReceivedFromClientMsg = recv(Socket, buf, MAX_DATA, 0);
		bytesReceivedFromClientMsgSum += bytesReceivedFromClientMsg;
		// Base64
		strMessage += buf;
	} while (bytesReceivedFromClientMsg > 0 && strMessage.find('$') == std::string::npos);

	return bytesReceivedFromClientMsgSum;
}

int SConnection::UpdateDeviceRow(ns__Device deviceInfo)
{
	DBConnection dbObject;

	unsigned long id = dbObject.UpdateDeviceRow(deviceInfo);

	return id;
}

int SConnection::CreateDeviceRow(ns__Device deviceInfo)
{
	DBConnection dbObject;

	if (dbObject.GetDeviceSetting(deviceInfo) == 0)
	{
		unsigned long id = dbObject.CreateDeviceRow(deviceInfo);
	}
	else
	{
		unsigned long id = dbObject.UpdateDeviceRow(deviceInfo);
	}

	return 1;
}

//************************************
// Method:    StartListon
// FullName:  SConnection::StartListon
// Access:    public 
// Returns:   int
// Qualifier:
//
// Description: It is run as a thread async
//************************************
int SConnection::StartListen()
{
	/* Main loop */
	while (1)
	{
		int size = sizeof(struct sockaddr);
		struct sockaddr_in their_addr;
		SOCKET newsock;
		ns__Device devObj;

		ZeroMemory(&their_addr, sizeof(struct sockaddr));
		newsock = accept(this->ListenSocket, (struct sockaddr*)&their_addr, &size);
		if (newsock == INVALID_SOCKET)
		{
			perror("accept\n");
		}
		else
		{
			/* Use the new socket */
			//uintptr_t thread;
			//std::cout << "Got a connection from " << inet_ntoa( their_addr.sin_addr ) << " on port " << ntohs( their_addr.sin_port ) << "\n";
			//std::cout << "New socket is " << newsock << "\n";

			devObj.deviceSerialNumber = 123456;
			Splitter spObj(inet_ntoa(their_addr.sin_addr), ".");
			devObj.deviceIP.ipFirst = this->toolsObject.toInteger(spObj[0]);
			devObj.deviceIP.ipSecond = this->toolsObject.toInteger(spObj[1]);
			devObj.deviceIP.ipThird = this->toolsObject.toInteger(spObj[2]);
			devObj.deviceIP.ipFourth = this->toolsObject.toInteger(spObj[3]);
			devObj.devicePort = ntohs(their_addr.sin_port);
			devObj.deviceSocket = newsock;

			// Generate add run thread and continue
			this->ThreadHandleDevice(devObj);

			/*thread = _beginthread(this->HandleDevice, 1024, NULL);
			if (thread == -1) {
			//fprintf(stderr, "Couldn't create thread: %d\n", GetLastError());
			closesocket(newsock);
			}*/
		}
	}

	return 1;
}

int SConnection::ThreadStartListen()
{
	// Create thread
	std::thread threadObject(&SConnection::StartListen, *this);

	// Start thread and continue
	threadObject.detach();

	return 1;
}

int SConnection::ThreadHandleDevice(ns__Device DeviceObject)
{
	// Create a thread
	std::thread threadObject(&SConnection::HandleDevice, this, DeviceObject);

	// Run thread and continue
	threadObject.detach();

	return 1;
}

int SConnection::GetDeviceStatus(ns__Device& DeviceObject)
{
	DeviceMap deviceMapInfo;
	int result = this->AddUpdateDeletSearchDeviceInList(DeviceObject, 8, deviceMapInfo);

	return (result > 0);

}

int SConnection::SetDeviceStatus(ns__Device& DeviceObject)
{
	SyntaxChecker parserObject;
	DeviceMap deviceMapInfo;
	// Generate string sending to device
	std::string strCommand = parserObject.GenerateCNFCommand(DeviceObject);
	//ns__Device deviceObject;
	if (this->AddUpdateDeletSearchDeviceInList(DeviceObject, 4, deviceMapInfo))
	{
		if (this->ApplayCommand(DeviceObject, strCommand))
		{
			return 1;
		}
	}

	return 0;
}

int SConnection::ApplayCommand(ns__Device DeviceObject, std::string& CommandString)
{
	int bytesReceivedFromClientMsg = 1;
	char buf[MAX_DATA] = { '\0' };

	DeviceMap deviceMapInfo;
	deviceMapInfo.CommandString = CommandString;

	// Save CNF command in device object to progress in device
	this->AddUpdateDeletSearchDeviceInList(DeviceObject, 5, deviceMapInfo);

	// While CNF not progressed and result not returned
	do
	{
		//Sleep(1000);
		// Get CNF result
		this->AddUpdateDeletSearchDeviceInList(DeviceObject, 7, deviceMapInfo);
	} while (deviceMapInfo.CommandString != "" && deviceMapInfo.CommandResurl == "");

	CommandString = deviceMapInfo.CommandResurl;

	//std::cout << deviceMapInfo.CommandString << std::endl;
	//std::cout << "CNF Result is: " << deviceMapInfo.CommandResurl << std::endl;

	// Update Status
	// Get result from device and read result to device error class object variables
	// Delete $ from end of command
	this->toolsObject.replaceAll(CommandString, "$", "");

	// Split received data (device serial number and result with > seperator)
	Splitter sp(CommandString, ">");

	// Two section is OK
	if (sp.size() == 2)
	{
		DeviceObject.deviceError.errorMessage = sp[1];
		DeviceObject.deviceError.errorNumber = 1;
	}
	else
	{
		DeviceObject.deviceError.errorMessage = "Error";
		DeviceObject.deviceError.errorNumber = 1;
	}

	return 1;
}

std::map<std::string, std::string> SConnection::GetSensorsName(std::string Input)
{
	return std::map<std::string, std::string>();
}

int SConnection::AddUpdateDeletSearchDeviceInList(ns__Device& NewDevice, int CommandType, DeviceMap& NewDeviceMap)
{
	int result = 0;
	/* Enter the critical section -- other threads are locked out */
	EnterCriticalSection(&(this->csDeviceList));

	switch (CommandType)
	{
		case 1: // Add
		{
			int i = 0;
			for (; i < deviceInfoList.size(); i++)
			{
				if (deviceInfoList[i].deviceSerialNumber == NewDevice.deviceSerialNumber)
				{
					deviceInfoList[i] = NewDevice;
					result = -1;
					break;
				}
			}

			if (result != -1)
			{
				deviceInfoList.push_back(NewDevice);
				DeviceMap deviceMapObj;
				deviceMap.insert(std::pair<int, DeviceMap>(NewDevice.deviceSerialNumber, deviceMapObj));
				result = 1;
			}
			break;
		}
		case 2: // Update
		{
			for (int i = 0; i < deviceInfoList.size(); i++)
			{
				if (deviceInfoList[i].deviceSerialNumber == NewDevice.deviceSerialNumber)
				{
					deviceInfoList[i] = NewDevice;
					result = 1;
					break;
				}
			}
			break;
		}
		case 3: // Delete
		{
			std::vector<ns__Device>::iterator it;
			for (it = deviceInfoList.begin(); it != deviceInfoList.end(); it++)
			{
				if (it->deviceSerialNumber == NewDevice.deviceSerialNumber)
				{
					deviceMap.erase(NewDevice.deviceSerialNumber);
					deviceInfoList.erase(it);
					result = 1;
					break;
				}
			}
			break;
		}
		case 4: // Search
		{
			result = 0;
			for (int i = 0; i < deviceInfoList.size(); i++)
			{
				if (deviceInfoList[i].deviceSerialNumber == NewDevice.deviceSerialNumber)
				{
					result = 1;
					break;
				}
			}
			break;
		}
		case 5: // Set CNF
		{
			for (int i = 0; i < deviceInfoList.size(); i++)
			{
				if (deviceInfoList[i].deviceSerialNumber == NewDevice.deviceSerialNumber)
				{
					deviceMap[deviceInfoList[i].deviceSerialNumber].CommandString = NewDeviceMap.CommandString;
					deviceMap[deviceInfoList[i].deviceSerialNumber].CommandResurl = "";
					result = 1;
					break;
				}
			}
			break;
		}
		case 6: // Set CNF result
		{
			for (int i = 0; i < deviceInfoList.size(); i++)
			{
				if (deviceInfoList[i].deviceSerialNumber == NewDevice.deviceSerialNumber)
				{
					deviceMap[deviceInfoList[i].deviceSerialNumber].CommandResurl = NewDeviceMap.CommandResurl;
					deviceMap[deviceInfoList[i].deviceSerialNumber].CommandString = "";
					result = 1;
					break;
				}
			}
			break;
		}
		case 7: // Get CNF
		{
			result = -1;
			for (int i = 0; i < deviceInfoList.size(); i++)
			{
				if (deviceInfoList[i].deviceSerialNumber == NewDevice.deviceSerialNumber)
				{
					NewDeviceMap.CommandString = deviceMap[deviceInfoList[i].deviceSerialNumber].CommandString;
					NewDeviceMap.CommandResurl = deviceMap[deviceInfoList[i].deviceSerialNumber].CommandResurl;
					break;
				}
			}
			break;
		}
		case 8: // Search
		{
			result = 0;
			for (int i = 0; i < deviceInfoList.size(); i++)
			{
				if (deviceInfoList[i].deviceSerialNumber == NewDevice.deviceSerialNumber)
				{
					NewDevice = deviceInfoList[i];
					result = 1;
					break;
				}
			}
			break;
		}
	}

	/* Leave the critical section -- other threads can now EnterCriticalSection() */
	LeaveCriticalSection(&(this->csDeviceList));

	return result;
}

void SConnection::InitiateDeviceStatus(ns__Device DeviceObject, std::string &ErrorsString)
{
	// Analyze all sensors data
	// (include min and max and thresholds)
	// and generate status string
	ErrorsString = DeviceObject.deviceError.errorMessage;

	if (ErrorsString == "")
	{
		// For every sensors
		if (DeviceObject.deviceSensors.size() > 0)
		{
			// Error index
			int counter = 1;
			for each(ns__Sensor sensor in DeviceObject.deviceSensors)
			{
				// Is a sensor with min max and threshold data
				//  Is multi sensor
				if (sensor.sensorType == ns__SensorType::Multi)
				{
					// Value is lower than minimum or upper than maximum
					if (sensor.sensorValue < sensor.sensorMinimumValue + sensor.sensorMinimumThreshold)
					{
						if (sensor.sensorValue < sensor.sensorMinimumValue)
						{
							if (counter != 1)
							{
								ErrorsString += ", ";
							}
							// Set error string
							ErrorsString += this->toolsObject.toString(counter) + "_" + sensor.sensorNikeName + "_Minimum";
							counter++;
						}
						else
						{
							if (counter != 1)
							{
								ErrorsString += ", ";
							}
							// value is between minimum and minimum threshold (warning area)
							// Set error string
							ErrorsString += this->toolsObject.toString(counter) + "_" + sensor.sensorNikeName + "_Minimum_threshold";
							counter++;
						}
					}
					else if (sensor.sensorValue > sensor.sensorMaximumValue - sensor.sensorMaximumThreshold)
					{
						if (sensor.sensorValue > sensor.sensorMaximumValue)
						{
							if (counter != 1)
							{
								ErrorsString += ", ";
							}
							// Set error string
							ErrorsString += this->toolsObject.toString(counter) + "_" + sensor.sensorNikeName + "_Maximum";
							counter++;
						}
						else
						{
							if (counter != 1)
							{
								ErrorsString += ", ";
							}
							// value is between minimum and minimum threshold (warning area)
							// Set error string
							ErrorsString += this->toolsObject.toString(counter) + "_" + sensor.sensorNikeName + "_Maximum_threshold";
							counter++;
						}
					}
				}
				else
				{
					if (sensor.sensorValue == 1)
					{
						if (counter != 1)
						{
							ErrorsString += ", ";
						}
						// Set error string
						ErrorsString += this->toolsObject.toString(counter) + "_" + sensor.sensorNikeName + "_Error";
						counter++;
					}
				}
			}
		}
	}
}

int SConnection::SaveDevieStatus(ns__Device DeviceObject, std::string DeviceStatusString)
{
	DBConnection dbObject;

	return dbObject.SaveDeviceStatus(DeviceObject, DeviceStatusString);
}