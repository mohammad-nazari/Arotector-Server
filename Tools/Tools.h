#pragma once
#ifndef _TOOLS_H_
#define _TOOLS_H_

#include "../ProtectorServer/Server_SoapStub.h"
#include "../ProtectorServer/Header.h"
#include "EnumString.h"
#include "../Security/Base64.h"
//#include "..\Split\Splitter.h"

// String support for Furious Five Master
Begin_Enum_String(ns__SimcardType)
{
	Enum_String(MCI);
	Enum_String(IranCell);
	Enum_String(RighTel);
	Enum_String(Taliya);
	Enum_String(UnknownSim);
}
End_Enum_String;

Begin_Enum_String(ns__SensorType)
{
	Enum_String(Mono);
	Enum_String(Multi);
}
End_Enum_String;

Begin_Enum_String(ns__UserType)
{
	Enum_String(Admin);
	Enum_String(Control);
	Enum_String(Monitor);
}
End_Enum_String;

Begin_Enum_String(ns__SensorName)
{
	Enum_String(TEMPERATURE);
	Enum_String(HUMIDITY);
	Enum_String(H2S);
	Enum_String(PRESSER);
	Enum_String(ACVOLTAGE);
	Enum_String(ACVOLTAGE1);
	Enum_String(ACVOLTAGE2);
	Enum_String(ACVOLTAGE3);
	Enum_String(ACAMPERE);
	Enum_String(ACAMPERE1);
	Enum_String(ACAMPERE2);
	Enum_String(ACAMPERE3);
	Enum_String(DCVOLTAGE);
	Enum_String(DCAMPERE);
	Enum_String(GAS);
	Enum_String(SMOKE);
	Enum_String(MAGNET);
	Enum_String(WATER);
	Enum_String(MOTION);
	Enum_String(DIGITAL);
}
End_Enum_String;

class DeviceMap
{
public:
	std::string CommandString;
	std::string CommandResurl;
protected:
private:
};

class Tools
{
public:
	__declspec(dllexport)Tools(void);
	__declspec(dllexport)~Tools(void);

	__declspec(dllexport)void replaceAll(std::string& str, const std::string& oldStr, const std::string& newStr);

	__declspec(dllexport)size_t replaceOne(std::string& str, const std::string& oldStr, const std::string& newStr, size_t position);

	__declspec(dllexport)size_t findOne(std::string str, const std::string& findStr, size_t position);

	__declspec(dllexport)std::string getCurrentDateTime();

	__declspec(dllexport)struct tm toTMStruct(std::string Input);

	__declspec(dllexport)std::string toUpper(std::string& str);

	__declspec(dllexport)std::string toLower(std::string& str);

	__declspec(dllexport)bool is_number(const std::string& str);

	__declspec(dllexport)bool is_numberWithLen(const std::string& s, int lenght);

	__declspec(dllexport)bool is_double(std::string const& s);

	__declspec(dllexport)int toInteger(std::string const& Input);

	__declspec(dllexport)std::string toString(int Input);

	__declspec(dllexport)std::string toString(int Input, int Length);
	__declspec(dllexport)int getDigitLenth(long int InputNumber);
	__declspec(dllexport)std::string toString(long long int Input);
	__declspec(dllexport)std::string toString(long long int Input, int Length);
	__declspec(dllexport)long long int toLongLong(std::string const& Input);

	__declspec(dllexport)bool checkFormat(std::string Input, std::string DataFormat);

	__declspec(dllexport)bool isInString(std::string Input, std::string ParentString);

	__declspec(dllexport)std::string SimCardTypeToString(ns__SimcardType SimCardType);
	__declspec(dllexport)bool SimCardTypeFromString(ns__SimcardType &SimcardType, std::string SimCardTypeName);

	__declspec(dllexport)std::string UserTypeToString(ns__UserType UserType);
	__declspec(dllexport)bool UserTypeFromString(ns__UserType &UserType, std::string UserTypeName);

	__declspec(dllexport)std::string SensorTypeToString(ns__SensorType SensorType);
	__declspec(dllexport)bool SensorTypeFromString(ns__SensorType &SensorType, std::string SensorTypeName);

	__declspec(dllexport)std::string SensorNameToString(ns__SensorName SensorType);
	__declspec(dllexport)bool SensorNameFromString(ns__SensorName &SensorType, std::string SensorTypeName);

	__declspec(dllexport)std::string ltrim(std::string const &s);
	__declspec(dllexport)std::string rtrim(std::string const &s);
	__declspec(dllexport)std::string trim(std::string const &s);

private:

	char * strptime(const char *buf, const char *fmt, struct tm *tm);

	static int conv_num(const char **buf, int *dest, int llim, int ulim);

	int strncasecmp(char *s1, char *s2, size_t n);
};

#endif //_TOOLS_H_