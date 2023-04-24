//Content of file "options.h": 
//gsoap ns service name: Options
//gsoap ns service namespace: Options
//gsoap ns schema namespace: Options

#include <string.h>
#include <vector>

#import "stlvector.h"

typedef std::string xsd__string;
typedef int xsd__int;
typedef bool xsd__boolean;
typedef double xsd__double;
typedef struct tm xsd__dateTime;
typedef long long xsd__long;

enum ns__SensorName
{
	GAS,
	TEMPERATURE,
	HUMIDITY,
	H2S,
	PRESSER,
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
	UNKNOWN
};

class ns__Sensor
{
	enum ns__SensorName sensorName = TEMPERATURE;
	xsd__int sensorCalibration = 5;
	xsd__int sensorMinimumValue = 0;
	xsd__int sensorMaximumValue = 100;
	xsd__int sensorMaximumThreshold = 10;
	xsd__int sensorMinimumThreshold = 10;
};

class ns__DataBaseInfo
{
	xsd__string	dataBaseInfoServerName = "localhost";
	xsd__int	dataBaseInfoPort = 3306;
	xsd__string	dataBaseInfoUserName = "root";
	xsd__string	dataBaseInfoPassWord = "123456";
	xsd__string	databaseDatabeseName = "protectordb";
};

class ns__Messages
{
	xsd__string messagesDescription; // Showing on setting form
	xsd__string messagesString;	// Show to user
	xsd__string messagesName; // Define name
	xsd__int	messagesCodeNumber;	// Message code number
};

class ns__ServerOptions
{
	ns__DataBaseInfo optionsDataBaseInfo;
	xsd__int optionsTimeRepeat = 3; // In Second
	std::vector<ns__Sensor> optionsSensorValues;
	std::vector<ns__Messages> optionsMessages;
};

