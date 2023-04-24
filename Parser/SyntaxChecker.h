#pragma once
#ifndef _SYNTAX_CHECKER_H_
#define _SYNTAX_CHECKER_H_

#include "../ProtectorServer/Server_SoapH.h"
#include "Header.h"

#include "../Split/Splitter.h"
#include "../Tools/Tools.h"
#include <map>

class SyntaxChecker
{

public:
	__declspec(dllexport)SyntaxChecker(void);
	__declspec(dllexport)~SyntaxChecker(void);

	__declspec(dllexport)bool CheckSTTSyntax(std::string& InputOrg);

	__declspec(dllexport)Tools get_toolsObject() const;

	__declspec(dllexport)void set_toolsObject(const Tools& tools);

	__declspec(dllexport)std::string get_errorData() const;

	__declspec(dllexport)void set_errorData(const std::string& cs);

	__declspec(dllexport)ns__Device get_deviceInfo() const;

	__declspec(dllexport)void set_deviceInfo(const ns__Device& ns__device);

	__declspec(dllexport)std::string GenerateCNFCommand(ns__Device DeviceObject);
private:

	bool CheckPhoneNumber(std::string Input);

	bool CheckSimcardType(std::string Input);

	bool CheckDate(std::string Input);

	bool CheckTime(std::string Input);

	bool CheckIp(std::string Input, ns__IP& IPObject);

	bool CheckDeviceNumber(std::string Input);

	bool CheckCommandType(std::string Input);

	bool CheckDeviceVersion(std::string Input);

	bool CheckSimcard(std::string Input);

	bool CheckDateTime(std::string Input);

	bool CheckServer(std::string Input);

	bool CheckGPRS(std::string Input);

	bool CheckSMSContact(std::string Input);

	bool CheckSMSConfig(std::string Input);

	bool CheckSMSCommand(std::string Input);

	bool CheckGSMCommand(std::string Input);

	bool CheckKeyboardCommand(std::string Input);

	bool CheckRelay(std::string Input, int index);

	bool CheckSensors(std::string Input, int index);

	std::map<std::string, ns__SensorName> GetSensorsName(std::string Input);
	std::map<std::string, ns__SensorName> dictionarySensorName;

	std::map<ns__SensorName, std::string> GetSensorsNikeName(std::string Input);
	bool CheckDeviceBuzzer(std::string Input);
	std::map<ns__SensorName, std::string> dictionarySensorNikeName;

private:
	Tools toolsObject;
	std::string errorData;
	ns__Device deviceInfo;
	ns__Sensor sensorTemp;

};

#endif //_SYNTAX_CHECKER_H_