#include "SyntaxChecker.h"

SyntaxChecker::SyntaxChecker(void)
{
	// Generate dictionary of sensors names
	this->dictionarySensorName = GetSensorsName(SENSORSNAMEMULTI);
	std::map<std::string, ns__SensorName> mapTemp = GetSensorsName(SENSORSNAMEMONO);
	this->dictionarySensorName.insert(mapTemp.begin(), mapTemp.end());

	this->dictionarySensorNikeName = GetSensorsNikeName(SENSORSNAMEMULTI);
	std::map<ns__SensorName, std::string> mapNTemp = GetSensorsNikeName(SENSORSNAMEMONO);
	this->dictionarySensorNikeName.insert(mapNTemp.begin(), mapNTemp.end());
}

SyntaxChecker::~SyntaxChecker(void)
{
}

bool SyntaxChecker::CheckDateTime(std::string Input)
{
	soap_default_tm(new soap(),&this->deviceInfo.deviceDateTime);
	Splitter sp(Input, ",");
	if(sp.size() == 2)
	{
		if(CheckDate(sp[0]))
		{
			if(CheckTime(sp[1]))
			{
				return true;
			}
		}
	}
	return false;
}

bool SyntaxChecker::CheckDate(std::string Input)
{
	this->deviceInfo.deviceDateTime.tm_year = 1900;
	this->deviceInfo.deviceDateTime.tm_mon = 1;
	this->deviceInfo.deviceDateTime.tm_mday = 1;

	Splitter sp(Input, "-");
	if(sp.size() == 3)
	{
		if(this->toolsObject.is_numberWithLen(sp[0], 2) || this->toolsObject.is_numberWithLen(sp[0], 4))
		{
			if(this->toolsObject.is_numberWithLen(sp[1], 2))
			{
				if(this->toolsObject.is_numberWithLen(sp[2], 2))
				{
					this->deviceInfo.deviceDateTime.tm_year = 100 + std::min(this->toolsObject.toInteger(sp[0]), 99);
					this->deviceInfo.deviceDateTime.tm_mon = std::min(this->toolsObject.toInteger(sp[1]), 11);
					this->deviceInfo.deviceDateTime.tm_mday = std::min(this->toolsObject.toInteger(sp[2]), 29);
					return true;
				}
			}
		}
	}

	return false;
}

bool SyntaxChecker::CheckTime(std::string Input)
{
	this->deviceInfo.deviceDateTime.tm_hour = 0;
	this->deviceInfo.deviceDateTime.tm_min = 0;
	this->deviceInfo.deviceDateTime.tm_sec = 0;

	Splitter sp(Input, ":");
	if(sp.size() == 2 || sp.size() == 3)
	{
		if(this->toolsObject.is_numberWithLen(sp[0], 2))
		{
			if(this->toolsObject.is_numberWithLen(sp[1], 2))
			{
				this->deviceInfo.deviceDateTime.tm_hour = std::min(this->toolsObject.toInteger(sp[0]), 11);
				this->deviceInfo.deviceDateTime.tm_min = std::min(this->toolsObject.toInteger(sp[1]), 59);
				if(sp.size() == 3)
				{
					if(this->toolsObject.is_numberWithLen(sp[1], 2))
					{
						this->deviceInfo.deviceDateTime.tm_sec = 0;
						return true;
					}
				}
				return true;
			}
		}
	}

	return false;
}

bool SyntaxChecker::CheckDeviceNumber(std::string Input)
{
	if(this->toolsObject.is_numberWithLen(Input, 8))
	{
		this->deviceInfo.deviceSerialNumber = this->toolsObject.toInteger(Input);
		return true;
	}
	return false;
}

bool SyntaxChecker::CheckCommandType(std::string Input)
{
	if(this->toolsObject.toUpper(Input) == "STT")
	{
		return true;
	}
	return false;
}

bool SyntaxChecker::CheckDeviceVersion(std::string Input)
{
	// split for version and device buzzer on or off
	Splitter sp(Input, ",");
	if(sp.size() == 2)
	{
		if(sp[0].size() > 0)
		{
			if(sp[0] == "-")
			{
				return true;
			}
			this->deviceInfo.deviceFirmWareVersion = Input;
			if(CheckDeviceBuzzer(sp[1]))
			{
				return true;
			}
		}
	}
	return false;
}

bool SyntaxChecker::CheckDeviceBuzzer(std::string Input)
{
	if(Input.size() > 0)
	{
		if(Input == "0" || Input == "1")
		{
			this->deviceInfo.deviceBuzzerOnOff = toolsObject.toInteger(Input);
			return true;
		}
	}
	return false;
}

bool SyntaxChecker::CheckSimcard(std::string Input)
{
	this->deviceInfo.deviceMobileInfo.mobileNumber = 0;
	this->deviceInfo.deviceMobileInfo.mobileSimCardType = MCI;
	this->deviceInfo.deviceMobileInfo.mobileSignalValue = 0;
	this->deviceInfo.deviceMobileInfo.mobileChargeValue = 0;

	if(Input == "-")
	{
		return true;
	}

	Splitter sp(Input, ",");
	if(sp.size() == 4)
	{
		if(CheckPhoneNumber(sp[0]))
		{
			if(CheckSimcardType(sp[1]))
			{
				if(this->toolsObject.is_number(sp[2]))
				{
					if(this->toolsObject.is_number(sp[3]))
					{
						this->deviceInfo.deviceMobileInfo.mobileNumber = this->toolsObject.toLongLong(sp[0]);
						this->toolsObject.SimCardTypeFromString(this->deviceInfo.deviceMobileInfo.mobileSimCardType, sp[1]);
						this->deviceInfo.deviceMobileInfo.mobileChargeValue = this->toolsObject.toInteger(sp[2]);
						this->deviceInfo.deviceMobileInfo.mobileSignalValue = this->toolsObject.toInteger(sp[3]);
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool SyntaxChecker::CheckPhoneNumber(std::string Input)
{
	this->deviceInfo.deviceMobileInfo.mobileNumber = 0;
	if(this->toolsObject.is_numberWithLen(Input, 10))
	{
		this->deviceInfo.deviceMobileInfo.mobileNumber = this->toolsObject.toLongLong(Input);
		return true;
	}
	return false;
}

bool SyntaxChecker::CheckSimcardType(std::string Input)
{
	if(this->toolsObject.SimCardTypeFromString(this->deviceInfo.deviceMobileInfo.mobileSimCardType, Input))
	{
		return true;
	}
	return false;
}

bool SyntaxChecker::CheckServer(std::string Input)
{
	this->deviceInfo.deviceServerInfo.serverPort = 0;
	this->deviceInfo.deviceServerInfo.serverIP.ipFirst = 0;
	this->deviceInfo.deviceServerInfo.serverIP.ipSecond = 0;
	this->deviceInfo.deviceServerInfo.serverIP.ipThird = 0;
	this->deviceInfo.deviceServerInfo.serverIP.ipFourth = 0;

	if(Input == "-")
	{
		return true;
	}

	Splitter sp(Input, ",");
	if(sp.size() == 2)
	{
		if(CheckIp(sp[0], this->deviceInfo.deviceServerInfo.serverIP))
		{
			if(this->toolsObject.is_number(sp[1]))
			{
				this->deviceInfo.deviceServerInfo.serverPort = this->toolsObject.toInteger(sp[1]);
				return true;
			}
		}
	}
	return false;
}

bool SyntaxChecker::CheckIp(std::string Input, ns__IP& IPObject)
{
	Splitter sp(Input, ".");

	IPObject.ipFirst = 0;
	IPObject.ipSecond = 0;
	IPObject.ipThird = 0;
	IPObject.ipFourth = 0;

	if(sp.size() == 4)
	{
		if(this->toolsObject.is_number(sp[0]) && this->toolsObject.toInteger(sp[0]) >= 0 && this->toolsObject.toInteger(sp[0]) <= 255)
		{
			if(this->toolsObject.is_number(sp[1]) && this->toolsObject.toInteger(sp[1]) >= 0 && this->toolsObject.toInteger(sp[1]) <= 255)
			{
				if(this->toolsObject.is_number(sp[2]) && this->toolsObject.toInteger(sp[2]) >= 0 && this->toolsObject.toInteger(sp[2]) <= 255)
				{
					if(this->toolsObject.is_number(sp[3]) && this->toolsObject.toInteger(sp[3]) >= 0 && this->toolsObject.toInteger(sp[3]) <= 255)
					{
						IPObject.ipFirst = this->toolsObject.toInteger(sp[0]);
						IPObject.ipSecond = this->toolsObject.toInteger(sp[1]);
						IPObject.ipThird = this->toolsObject.toInteger(sp[2]);
						IPObject.ipFourth = this->toolsObject.toInteger(sp[3]);
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool SyntaxChecker::CheckGPRS(std::string Input)
{
	this->deviceInfo.deviceGprsInfo.gprsPort = 0;
	this->deviceInfo.deviceGprsInfo.gprsIP.ipFirst = 0;
	this->deviceInfo.deviceGprsInfo.gprsIP.ipSecond = 0;
	this->deviceInfo.deviceGprsInfo.gprsIP.ipThird = 0;
	this->deviceInfo.deviceGprsInfo.gprsIP.ipFourth = 0;

	if(Input == "-")
	{
		return true;
	}

	Splitter sp(Input, ",");
	if(sp.size() == 2)
	{
		if(CheckIp(sp[0], this->deviceInfo.deviceGprsInfo.gprsIP))
		{
			if(this->toolsObject.is_number(sp[1]))
			{
				this->deviceInfo.deviceGprsInfo.gprsPort = this->toolsObject.toInteger(sp[1]);
				return true;
			}
		}
	}
	return false;
}

bool SyntaxChecker::CheckSMSContact(std::string Input)
{
	if(Input == "-")
	{
		return true;
	}

	Splitter sp(Input, ",");
	for(int i = 0; i < sp.size(); i++)
	{
		if(!this->toolsObject.is_numberWithLen(sp[i], 10))
		{
			return false;
		}
		this->deviceInfo.deviceSMSContact.push_back(this->toolsObject.toLongLong(sp[i]));
	}
	return true;
}

bool SyntaxChecker::CheckSMSConfig(std::string Input)
{
	if(Input == "-")
	{
		return true;
	}

	Splitter sp(Input, ",");
	for(int i = 0; i < sp.size(); i++)
	{
		if(!this->toolsObject.is_numberWithLen(sp[i], 10))
		{
			return false;
		}
		this->deviceInfo.deviceSMSConfig.push_back(this->toolsObject.toLongLong(sp[i]));
	}
	return true;
}

bool SyntaxChecker::CheckSMSCommand(std::string Input)
{
	this->deviceInfo.deviceSMSCommand = false;
	if(Input == "-")
	{
		return true;
	}

	if(Input == "0" || Input == "1")
	{
		this->deviceInfo.deviceSMSCommand = this->toolsObject.toInteger(Input);
		return true;
	}
	return false;
}

bool SyntaxChecker::CheckGSMCommand(std::string Input)
{
	this->deviceInfo.deviceGSMCommand = false;
	if(Input == "-")
	{
		return true;
	}

	if(Input == "0" || Input == "1")
	{
		this->deviceInfo.deviceGSMCommand = this->toolsObject.toInteger(Input);
		return true;
	}
	return false;
}

bool SyntaxChecker::CheckKeyboardCommand(std::string Input)
{
	this->deviceInfo.deviceKeyBoardCommand = false;
	if(Input == "-")
	{
		return true;
	}

	if(Input == "0" || Input == "1")
	{
		this->deviceInfo.deviceKeyBoardCommand = this->toolsObject.toInteger(Input);
		return true;
	}
	return false;
}

bool SyntaxChecker::CheckRelay(std::string Input, int index)
{
	sensorTemp.sensorRelay.relayIndex = 0;
	sensorTemp.sensorRelay.relayOnOff = false;
	if(Input == "-")
	{
		return true;
	}

	Splitter sp(Input, ":");
	if(sp.size() == 2)
	{
		if(sp[0] == "" || this->toolsObject.is_number(sp[0]))
		{
			sensorTemp.sensorRelay.relayIndex = this->toolsObject.toInteger(sp[0]);
			if(sp[1] == "0" || sp[1] == "1")
			{
				sensorTemp.sensorRelay.relayOnOff = (this->toolsObject.toInteger(sp[1]) != 0 ? true : false);
				return true;
			}
		}
	}
	return false;
}

bool SyntaxChecker::CheckSensors(std::string Input, int index)
{
	if(Input == "-")
	{
		return true;
	}

	int Index = 0;

	Splitter sp(Input, ",");
	if(sp.size() == 10 || sp.size() == 5)
	{
		// Check name of sensor in dictionaries
		// If it not exist use abbreviation sensor name received from device
		sensorTemp.sensorNikeName = toolsObject.toUpper(sp[Index]);
		if(this->dictionarySensorName.find(sensorTemp.sensorNikeName) != this->dictionarySensorName.end())
		{
			sensorTemp.sensorName = this->dictionarySensorName[sensorTemp.sensorNikeName];
		}
		else if(this->dictionarySensorName.find(sensorTemp.sensorNikeName) != this->dictionarySensorName.end())
		{
			sensorTemp.sensorName = this->dictionarySensorName[sensorTemp.sensorNikeName];
		}

		Index++;
		if(sp[Index] == "-" || this->toolsObject.is_double(sp[Index]))
		{
			sensorTemp.sensorValue = this->toolsObject.toInteger(sp[Index++]);

			if(sp.size() == 10)
			{
				// Is multi sensor with min and max values
				sensorTemp.sensorType = Multi;
				if(this->toolsObject.is_number(sp[Index]))
				{
					sensorTemp.sensorCalibration = this->toolsObject.toInteger(sp[Index++]);
					if(this->toolsObject.is_number(sp[Index]))
					{
						sensorTemp.sensorMinimumThreshold = this->toolsObject.toInteger(sp[Index++]);
						if(this->toolsObject.is_number(sp[Index]))
						{
							sensorTemp.sensorMaximumThreshold = this->toolsObject.toInteger(sp[Index++]);
							if(this->toolsObject.is_number(sp[Index]))
							{
								sensorTemp.sensorMinimumValue = this->toolsObject.toInteger(sp[Index++]);
								if(this->toolsObject.is_number(sp[Index]))
								{
									sensorTemp.sensorMaximumValue = this->toolsObject.toInteger(sp[Index++]);
								}
								else
								{
									this->errorData = "Sensor max Threshold";
								}
							}
							else
							{
								this->errorData = "Sensor min Threshold";
							}
						}
						else
						{
							this->errorData = "Sensor max offset";
						}
					}
					else
					{
						this->errorData = "Sensor min offset";
					}
				}
				else
				{
					this->errorData = "Sensor Calibration";
				}
			}
			else
			{
				// Is mono sensor
				sensorTemp.sensorType = Mono;
			}
			if(sp[Index] == "0" || sp[Index] == "1")
			{
				sensorTemp.sensorSMSOnOff = this->toolsObject.toInteger(sp[Index++]);
				if(sp[Index] == "0" || sp[Index] == "1")
				{
					sensorTemp.sensorBuzzerOnOff = this->toolsObject.toInteger(sp[Index++]);
					if(CheckRelay(sp[Index], index))
					{
						this->deviceInfo.deviceSensors.push_back(sensorTemp);
						return true;
					}
					else
					{
						this->errorData = "Sensor relay";
					}
				}
				else
				{
					this->errorData = "Sensor sms";
				}
			}
			else
			{
				this->errorData = "Sensor sms";
			}
		}
		else
		{
			this->errorData = "Sensor value";
		}
	}
	else
	{
		this->errorData = "Sensor Size is not equal to 9 or 5";
	}
	return false;
}

bool SyntaxChecker::CheckSTTSyntax(std::string& InputOrg)
{
	this->deviceInfo.deviceSensors.clear();
	this->deviceInfo.deviceSMSConfig.clear();
	this->deviceInfo.deviceSMSContact.clear();

	this->deviceInfo.deviceError.errorNumber = 0;
	this->deviceInfo.deviceError.errorType = Normal;
	this->deviceInfo.deviceError.errorMessage.clear();

	std::string Input = InputOrg;

	this->toolsObject.replaceOne(Input, "\n", "", 0);
	this->toolsObject.replaceOne(Input, "\r", "", 0);
	this->toolsObject.replaceOne(Input, "$", "", 0);

	std::string a = "";

	int index = 0;
	Splitter sp(Input, ">");
	if(sp.size() > 12)
	{
		if(CheckDeviceNumber(sp[index++]))
		{
			if(CheckCommandType(sp[index++]))
			{
				if(CheckDeviceVersion(sp[index++]))
				{
					if(CheckSimcard(sp[index++]))
					{
						if(CheckDateTime(sp[index++]))
						{
							if(CheckServer(sp[index++]))
							{
								if(CheckGPRS(sp[index++]))
								{
									if(CheckSMSContact(sp[index++]))
									{
										if(CheckSMSConfig(sp[index++]))
										{
											if(CheckSMSCommand(sp[index++]))
											{
												if(CheckGSMCommand(sp[index++]))
												{
													if(CheckKeyboardCommand(sp[index++]))
													{
														for(int i = index; i < sp.size(); i++)
														{
															if(!CheckSensors(sp[i], i - index))
															{
																return false;
															}
														}
														return true;
													}
													else
													{
														this->errorData = "Config";
													}
												}
												else
												{
													this->errorData = "Config";
												}
											}
											else
											{
												this->errorData = "Contact";
											}
										}
										else
										{
											this->errorData = "GPRS";
										}
									}
									else
									{
										this->errorData = "Date Time";
									}
								}
								else
								{
									this->errorData = "SIM card";
								}
							}
							else
							{
								this->errorData = "Command type";
							}
						}
						else
						{
							this->errorData = "Device number";
						}
					}
					else
					{
						this->errorData = "Size is not equal to 15";
					}
				}
				else
				{
					this->errorData = "SIM card";
				}
			}
			else
			{
				this->errorData = "Command type";
			}
		}
		else
		{
			this->errorData = "Device number";
		}
	}
	else
	{
		this->errorData = "Size is not equal to 20";
	}

	return false;
}

Tools SyntaxChecker::get_toolsObject() const
{
	return toolsObject;
}

void SyntaxChecker::set_toolsObject(const Tools& tools)
{
	toolsObject = tools;
}

std::string SyntaxChecker::get_errorData() const
{
	return errorData;
}

void SyntaxChecker::set_errorData(const std::string& cs)
{
	errorData = cs;
}

ns__Device SyntaxChecker::get_deviceInfo() const
{
	return this->deviceInfo;
}

void SyntaxChecker::set_deviceInfo(const ns__Device& ns__device)
{
	this->deviceInfo = ns__device;
}


//************************************
// Method:    GenerateCNFCommand
// FullName:  SConnection::GenerateCNFCommand
// Access:    private 
// Returns:   std::string
// Qualifier:
// Parameter: ns__Device DeviceObject
//
// Description: Generate CNF Command string sending to Device
//************************************
std::string SyntaxChecker::GenerateCNFCommand(ns__Device DeviceObject)
{
	Tools toolsObject;

	std::string strCommand = "";

	//deviceNo[8digit]
	strCommand += toolsObject.toString(DeviceObject.deviceSerialNumber);

	strCommand += ">";

	// CNF
	strCommand += "CNF"; // Add CNF

	strCommand += ">";
	//date(y-m-d)
	strCommand += toolsObject.toString(DeviceObject.deviceDateTime.tm_year - 2000, 2) + "-" + toolsObject.toString(DeviceObject.deviceDateTime.tm_mon, 2) + "-" + toolsObject.toString(DeviceObject.deviceDateTime.tm_mday, 2);

	strCommand += ",";
	//time(HH:mm)
	strCommand += toolsObject.toString(DeviceObject.deviceDateTime.tm_hour, 2) + ":" + toolsObject.toString(DeviceObject.deviceDateTime.tm_min, 2);

	if(DeviceObject.deviceServerInfo.serverIP.ipFirst > 0 && DeviceObject.deviceServerInfo.serverIP.ipFirst < 256)
	{
		strCommand += ">";
		//serverIp['3digit.3digit.3digit.3digit']
		strCommand += toolsObject.toString(DeviceObject.deviceServerInfo.serverIP.ipFirst, 3) + "." + toolsObject.toString(DeviceObject.deviceServerInfo.serverIP.ipSecond, 3) + "." + toolsObject.toString(DeviceObject.deviceServerInfo.serverIP.ipThird, 3) + "." + toolsObject.toString(DeviceObject.deviceServerInfo.serverIP.ipFourth, 3);

		strCommand += ",";
		//serverPort[1-35535]
		strCommand += toolsObject.toString(DeviceObject.deviceServerInfo.serverPort);
	}
	else
	{
		//strCommand += ">-";
		strCommand += ">127.0.0.1,50001";
	}

	if(DeviceObject.deviceGprsInfo.gprsIP.ipFirst > 0 && DeviceObject.deviceGprsInfo.gprsIP.ipFirst < 256)
	{
		strCommand += ">";
		//GPRSIp['3digit.3digit.3digit.3digit']
		strCommand += toolsObject.toString(DeviceObject.deviceGprsInfo.gprsIP.ipFirst, 3) + "." + toolsObject.toString(DeviceObject.deviceGprsInfo.gprsIP.ipSecond, 3) + "." + toolsObject.toString(DeviceObject.deviceGprsInfo.gprsIP.ipThird, 3) + "." + toolsObject.toString(DeviceObject.deviceGprsInfo.gprsIP.ipFourth, 3);

		strCommand += ",";
		//GPRSPort[1-35535]
		strCommand += toolsObject.toString(DeviceObject.deviceGprsInfo.gprsPort);
	}
	else
	{
		strCommand += ">-";
	}

	strCommand += ">";
	//setFlags[(18)[0|1]](8)
	//strCommand += DeviceObject.deviceFlags;
	// Reset Device
	strCommand += DeviceObject.deviceReset ? "1" : "0";
	// Device Buzzer On or Off
	strCommand += DeviceObject.deviceBuzzerOnOff ? "1" : "0";
	strCommand += "00000000000000000";

	for each (ns__Sensor sensor in DeviceObject.deviceSensors)
	{
		strCommand += ">";
		//sensor [GAS|TEM|HUM|H2S|PRS|ACV1|ACV2|ACV3|ACA1|ACA2|ACA3|DCV|DCA|SMK|MAG|WAT|MOT|DIG]
		strCommand += this->toolsObject.toLower(this->dictionarySensorNikeName[sensor.sensorName]);

		// [GAS|TEM|HUM|H2S|PRS|ACV1|ACV2|ACV3|ACA1|ACA2|ACA3|DCV|DCA] Have more options
		if(sensor.sensorType == Multi)
		{
			strCommand += ",";
			//calibration[1-100]
			strCommand += toolsObject.toString(sensor.sensorCalibration);

			strCommand += ",";
			//minOffsetValue[1-100]
			strCommand += toolsObject.toString(sensor.sensorMinimumThreshold);

			strCommand += ",";
			//maxOffsetValue[1-100]
			strCommand += toolsObject.toString(sensor.sensorMaximumThreshold);

			strCommand += ",";
			//minValue[0-150]
			strCommand += toolsObject.toString(sensor.sensorMinimumValue);

			strCommand += ",";
			//maxValue[0-150]
			strCommand += toolsObject.toString(sensor.sensorMaximumValue);
		}

		strCommand += ",";
		//smsOnOff[0|1]
		strCommand += toolsObject.toString(sensor.sensorSMSOnOff);

		strCommand += ",";
		//alarmOnOff[0|1]
		strCommand += toolsObject.toString(sensor.sensorBuzzerOnOff);

		// Relay index is between 1 to 5
		if(sensor.sensorRelay.relayIndex > 0 && sensor.sensorRelay.relayIndex < 6)
		{
			strCommand += ",";
			//relayIndex[3digit]
			strCommand += toolsObject.toString(sensor.sensorRelay.relayIndex);

			strCommand += ":";
			//relay[1|0]
			strCommand += toolsObject.toString(sensor.sensorRelay.relayOnOff);
		}
		else
		{
			strCommand += ",-";
		}

		strCommand += ",";
		//LedFlag[1|0]
		strCommand += toolsObject.toString(sensor.sensorLEDFlag);
	}

	strCommand += "$";

	return strCommand;
}


std::map<std::string, ns__SensorName> SyntaxChecker::GetSensorsName(std::string Input)
{
	// Initiate to empty map
	std::map<std::string, ns__SensorName> outPut = {};
	// Split input by ,
	Splitter sp(Input, ",");
	if(sp.size() > 0)
	{
		Splitter sp2("test", "test");
		for(int i = 0; i < sp.size(); i++)
		{
			if(sp2.size() == 2)
			{
				sp2.reset(sp[i], ">");
				toolsObject.SensorNameFromString(outPut[toolsObject.toUpper(sp2[0])], toolsObject.toUpper(sp2[1]));
			}
		}
	}

	return outPut;
}

std::map<ns__SensorName, std::string> SyntaxChecker::GetSensorsNikeName(std::string Input)
{
	// Initiate to empty map
	std::map<ns__SensorName, std::string> outPut = {};
	ns__SensorName sensorNameTemp;
	// Split input by ,
	Splitter sp(Input, ",");
	if(sp.size() > 0)
	{
		Splitter sp2("test", "test");
		for(int i = 0; i < sp.size(); i++)
		{
			if(sp2.size() == 2)
			{
				sp2.reset(sp[i], ">");
				toolsObject.SensorNameFromString(sensorNameTemp, toolsObject.toUpper(sp2[1]));
				outPut[sensorNameTemp] = toolsObject.toUpper(sp2[0]);
			}
		}
	}

	return outPut;
}
