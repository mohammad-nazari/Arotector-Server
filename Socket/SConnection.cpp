#include "SConnection.h"

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
	this->commandCounter++;

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
	int nTimeout = TIMERCOUNTER;
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
	this->commandCounter++;

	ns__Device deviceInfo;

	/* send(), recv(), closesocket() */
	Base64 base64Object;
	SyntaxChecker parserObject;

	Input.deviceSerialNumber = this->IsDevice((SOCKET)Input.deviceSocket);

	// It is a user
	if (Input.deviceSerialNumber > 0)
	{
		int bytesReceivedFromClientMsg = 1;
		char buf[MAX_DATA] = { '\0' };

		std::string strMessage = std::to_string(Input.deviceSerialNumber) + ">STT$";

		// Get Device Status
		int bytes_sent = this->WriteData((SOCKET)Input.deviceSocket, strMessage.c_str());
		if (bytes_sent == SOCKET_ERROR)
		{
			std::cerr << "Error :  " << WSAGetLastError() << std::endl;
			return -1;
		}

		bytesReceivedFromClientMsg = this->ReadData((SOCKET)Input.deviceSocket, strMessage);
		if (bytesReceivedFromClientMsg == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAECONNRESET)
				std::cerr << "recv() Error :  " << WSAGetLastError() << std::endl;
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

					std::cout << "Serial number: (" << deviceInfo.deviceSerialNumber << ") IP Value: (" << deviceInfo.deviceIP.ipFirst << deviceInfo.deviceIP.ipSecond << deviceInfo.deviceIP.ipThird << deviceInfo.deviceIP.ipFourth << ") Port number: (" << deviceInfo.devicePort << ") Socket number: (" << deviceInfo.deviceSocket << ")" << std::endl;

					/*
					* Check device is in list or no
					* It is old device
					* Just add to list of sockets
					*/
					int existDevice = this->AddUpdateDeletSearchDeviceInList(deviceInfo, 4);

					// Device be added in the past and a thread exist for that currently
					if (existDevice == 1)
					{
						// Just update device info in database
						if (this->UpdateDeviceRow(deviceInfo) > 0)
						{
							// Add to device list
							existDevice = this->AddUpdateDeletSearchDeviceInList(deviceInfo, 1);
						}
						else
						{
							std::cerr << "\nError :  " << WSAGetLastError() << std::endl;
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
							existDevice = this->AddUpdateDeletSearchDeviceInList(deviceInfo, 1);
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
					if (existDevice != -1)
					{
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
							this->commandCounter++;

							/*
							* Check device is in database or no
							* It is old device
							* Just add to list of sockets
							*/
							existDevice = this->AddUpdateDeletSearchDeviceInList(deviceInfo, 4);

							/*
							* Every 1 second we get a status
							* from device and save it
							* for send to user for every STT command
							*/
							Sleep(TIMERCOUNTER - 3000); // Wait to send next STT command

							if (existDevice == 1)
							{
								bytesReceivedFromClientMsg = 1;
								buf[0] = '\0';
								strMessage = std::to_string(deviceInfo.deviceSerialNumber) + ">STT$";

								// Update Status
								bytes_sent = this->WriteData(deviceInfo.deviceSocket, strMessage.c_str());

								bytesReceivedFromClientMsg = this->ReadData(deviceInfo.deviceSocket, strMessage);
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

											this->AddUpdateDeletSearchDeviceInList(deviceInfo, 2);
										}
										else
										{
											deviceInfo.deviceError.errorMessage = std::to_string(deviceInfo.deviceSerialNumber) + ">NOK$";
											AddUpdateDeletSearchDeviceInList(deviceInfo, 2);
											tryCounter++;	// Add try
										}
									}
									else
									{
										deviceInfo.deviceError.errorMessage = std::to_string(deviceInfo.deviceSerialNumber) + ">NOK$";
										AddUpdateDeletSearchDeviceInList(deviceInfo, 2);
										tryCounter++;	// Add try
									}
								}
								else
								{
									deviceInfo.deviceError.errorMessage = std::to_string(deviceInfo.deviceSerialNumber) + ">NOK$";
									AddUpdateDeletSearchDeviceInList(deviceInfo, 2);
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
						if (this->AddUpdateDeletSearchDeviceInList(deviceInfo, 3))
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
	Tools toolsObject;
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
			//std::cerr << "recv() Error :  " << WSAGetLastError( ) << std::endl;
			return -1;
	}
	if (bytesReceivedFromClientMsg != 0)
	{
		//std::cout << NEWMESSAE2USER << strMessage << std::endl;

		Splitter sp(strMessage, ">");

		if (sp.size() == 2 && toolsObject.toUpper(sp[1]) == "DEV$")
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
	//std::string MessageData = Decode(strMessage);
	std::string MessageData = strMessage;

	bytes_sent = send(Socket, MessageData.c_str(), (int)MessageData.size(), 0);
	total_bytes_sent += bytes_sent;

	while (bytes_sent > 0 && total_bytes_sent < MessageData.size())
	{
		std::string strMessageData = MessageData.substr(total_bytes_sent, MessageData.size() - total_bytes_sent);
		bytes_sent = send(Socket, strMessageData.c_str(), (int)strMessageData.size(), 0);
		total_bytes_sent += bytes_sent;
	}
	// Check Data Received
	if (bytes_sent == SOCKET_ERROR)
	{
		//std::cerr << "Error :  " << WSAGetLastError( ) << std::endl;
	}

	return bytes_sent;
}

int SConnection::ReadData(SOCKET Socket, std::string& strMessage)
{
	char buf[MAX_DATA] = { '\0' };
	int bytesReceivedFromClientMsg = 1;

	while (bytesReceivedFromClientMsg > 0)
	{
		strMessage.clear();
		memset(buf, 0, MAX_DATA);  //clear the variable
		bool OK = true;
		while (OK)
		{
			bytesReceivedFromClientMsg = recv(Socket, buf, MAX_DATA, 0);
			if (bytesReceivedFromClientMsg == SOCKET_ERROR)
			{
				break; //break the look to take the next connection
			}
			if (bytesReceivedFromClientMsg == 0)
			{
				// Connection closed
				break;
			}
			for (int i = 0; i < strlen(buf); i++)
			{
				if (buf[i] == '$')
				{
					strMessage.push_back(buf[i]);
					OK = false;
					break;
				}
				else
				{
					strMessage.push_back(buf[i]);
				}
			}
		}
		if (!OK)
		{
			break;
		}
	}

	// Base64
	//strMessage = Encode(strMessage);

	return bytesReceivedFromClientMsg;
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
	this->commandCounter++;

	Tools toolsObject;

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
			devObj.deviceIP.ipFirst = toolsObject.toInteger(spObj[0]);
			devObj.deviceIP.ipSecond = toolsObject.toInteger(spObj[1]);
			devObj.deviceIP.ipThird = toolsObject.toInteger(spObj[2]);
			devObj.deviceIP.ipFourth = toolsObject.toInteger(spObj[3]);
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
	this->commandCounter++;
	
	// Create thread
	std::thread threadObject(&SConnection::StartListen, *this);

	// Start thread and continue
	threadObject.detach();

	return 1;
}

int SConnection::ThreadHandleDevice(ns__Device DeviceObject)
{
	this->commandCounter++;

	// Create a thread
	std::thread threadObject(&SConnection::HandleDevice, this, DeviceObject);

	// Run thread and continue
	threadObject.detach();

	return 1;
}

int SConnection::GetDeviceStatus(ns__Device& DeviceObject)
{
	int result = this->AddUpdateDeletSearchDeviceInList(DeviceObject, 4);

	return (result > 0);

}

int SConnection::SetDeviceStatus(ns__Device& DeviceObject)
{
	SyntaxChecker parserObject;
	// Generate string sending to device
	std::string strCommand = parserObject.GenerateCNFCommand(DeviceObject);
	ns__Device deviceObject;
	if (this->GetDeviceStatus(deviceObject))
	{
		if (this->ApplayCommand(deviceObject, strCommand) > 1)
		{
			return 1;
		}
	}

	return 0;
}

int SConnection::ApplayCommand(ns__Device DeviceObject, std::string CommandString)
{
	Tools toolsObject;

	int bytesReceivedFromClientMsg = 1;
	char buf[MAX_DATA] = { '\0' };

	// Update Status
	int bytes_sent = this->WriteData((SOCKET)DeviceObject.deviceSocket, CommandString.c_str());
	if (bytes_sent == SOCKET_ERROR)
	{
		//std::cerr << "Error :  " << WSAGetLastError( ) << std::endl;

		DeviceObject.deviceError.errorMessage = DEVICECONNECTIONMESSAGE;
		DeviceObject.deviceError.errorNumber = DEVICECONNECTIONNUMBER;

		return 0;
	}
	else
	{
		//std::cout << SERVERMESSAGE2DEVICE << CommandString << std::endl;

		bytesReceivedFromClientMsg = this->ReadData((SOCKET)DeviceObject.deviceSocket, CommandString);
		if (bytesReceivedFromClientMsg == SOCKET_ERROR)
		{
			// Could not connect to device
			if (WSAGetLastError() != WSAECONNRESET)
				//std::cerr << "recv() Error :  " << WSAGetLastError( ) << std::endl;

				DeviceObject.deviceError.errorMessage = DEVICECONNECTIONMESSAGE;
			DeviceObject.deviceError.errorNumber = DEVICECONNECTIONNUMBER;

			return 0;
		}
		else
		{
			// Get result from device and read result to device error class object variables
			// Delete $ from end of command
			toolsObject.replaceAll(CommandString, "$", "");

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
				DeviceObject.deviceError.errorMessage = sp[1];
				DeviceObject.deviceError.errorNumber = 1;
			}
		}
	}

	return 1;
}

int SConnection::AddUpdateDeletSearchDeviceInList(ns__Device& NewDevice, int CommandType)
{
	int result = 0;
	/* Enter the critical section -- other threads are locked out */
	EnterCriticalSection(&(this->csDeviceList));

	switch (CommandType)
	{
	case 1: // Add
	{
		int i = 0;
		for (; i < this->deviceInfoList.size(); i++)
		{
			if (this->deviceInfoList[i].deviceSerialNumber == NewDevice.deviceSerialNumber)
			{
				this->deviceInfoList[i] = NewDevice;
				result = -1;
				break;
			}
		}

		if (result != -1)
		{
			this->deviceInfoList.push_back(NewDevice);
			result = 1;
		}
		break;
	}
	case 2: // Update
	{
		for (int i = 0; i < this->deviceInfoList.size(); i++)
		{
			if (this->deviceInfoList[i].deviceSerialNumber == NewDevice.deviceSerialNumber)
			{
				this->deviceInfoList[i] = NewDevice;
				result = 1;
				break;
			}
		}
		break;
	}
	case 3: // Delete
	{
		std::vector<ns__Device>::iterator it;
		for (it = this->deviceInfoList.begin(); it != this->deviceInfoList.end(); it++)
		{
			if (it->deviceSerialNumber == NewDevice.deviceSerialNumber)
			{
				this->deviceInfoList.erase(it);
				result = 1;
				break;
			}
		}
		break;
	}
	case 4: // Search
	{
		result = 0;
		for (int i = 0; i < this->deviceInfoList.size(); i++)
		{
			if (this->deviceInfoList[i].deviceSerialNumber == NewDevice.deviceSerialNumber)
			{
				NewDevice = this->deviceInfoList[i];
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
