#pragma once
#ifndef _HEADER_H_
#define	_HEADER_H_
/*
* File:   Header.h
* Author: root
*
* Created on May 28, 2014, 7:26 PM
*/

#pragma once

#if defined(min)
#define llvm_min_bug min
#undef min
#endif
#if defined(max)
#define llvm_max_bug max
#undef max
#endif

#pragma warning(disable: 4146) // warning C4146: unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable: 4624) // warning C4624: 'llvm::AugmentedUse' : destructor could not be generated because a base class destructor is inaccessible
#pragma warning(disable: 4355) // warning C4355: 'this' : used in base member initializer list
#pragma warning(disable: 4800) // warning C4800: 'const unsigned int' : forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable: 4996) // warning C4996: 'std::_Copy_impl': Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. To disable this warning, use -D_Sclan::SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'
#pragma warning(disable: 4244) // warning C4244: 'return' : conversion from 'uint64_t' to 'unsigned int', possible loss of data

#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <conio.h>
#include <vector>
#include <process.h>
#include <algorithm>
#include <iterator>
#include <ctime>
#include <regex>
#include <cstdlib>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fstream>
#include <wchar.h>
#include <iomanip>
#include <thread>
#include <map>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <exception>
#include <ctype.h>
#include <stdlib.h>

typedef unsigned char uchar;

// Timer
#define CONNECTIONTIMEOUT	5000
#define SLEEPTIMEVALUE		1000

// Number of threads to alive
#define BACKLOG 4096    // Max. request backlog

// SOAP timeout data
#define SEND_TIMEOUT		60
#define RECV_TIMEOUT		60
#define CONNECT_TIMEOUT		30
#define ACCEPT_TIMEOUT		30

// Sensors 
#define COMMANDTYPE		"DEV,DON,ERR,STT,MEM,END,YOU,AUT,USR,CNF,INF,MAN,SMS,GPS,KYB,MBL,CHC"
#define SENSORSNAMEMULTI	"TEM>TEMPERATURE,HUM>HUMIDITY,H2S>H2S,PRS>PRESSER,ACV>ACVOLTAGE,ACV1>ACVOLTAGE1,ACV2>ACVOLTAGE2,ACV3>ACVOLTAGE3,ACA>ACAMPERE,ACA1>ACAMPERE1,ACA2>ACAMPERE2,ACA3>ACAMPERE3,DCV>DCVOLTAGE,DCA>DCAMPERE"
#define SENSORSNAMEMONO	"GAS>GAS,SMK>SMOKE,MAG>MAGNET,WAT>WATER,MOT>MOTION,DIG>DIGITAL"

#define DEFAULT_PORT	"50001"		/* Port to listen on */

#define DATABASESERVER		"localhost"
#define DATABASEUSER		"protector"
#define DATABASEPASSWORD	"Protector@1395"
#define DATABASENAME		"protectordb"

#define MAX_DATA		4096	// Maximum bytes that could be sent or received
#define BASE64ENCODING	0		// Make base64 encoding
#define ENCRIPTIONDATA	0		// Make encrypt data

// Commands Types
#define STTCOMMAND		700
#define CNFCOMMAND		710
#define DONCOMMAND		720
#define ERRCOMMAND		730
#define ENDCOMMAND		740

// Database Query commands
#define SELECTDEVICEIP			"SELECT * FROM `device` WHERE `DevIP`='%s'"
#define SELECTDEVICEINFO		"SELECT * FROM `device` WHERE `DevSerialNumber` = '%s'"
#define SELECTDEVICEINFOALL		"SELECT * FROM `device`"
#define SELECTDEVICESERIAL		"SELECT * FROM `device` WHERE `DevSerialNumber`='%s'"
#define SELECTCITYNAME			"SELECT DISTINCT `DevCity` FROM `device`"
#define SELECTLOCATIONNAME		"SELECT DISTINCT `DevLocation` FROM `device` WHERE `DevCity`='%s'"

#define INSERTDEVICE			"INSERT INTO `device`(`DevSerialNumber`,`DevName`, `DevCity`, `DevLocation`, `DevSocketNumber`, `DevIP`, `DevPort`, `DevInstallDate`, `DevStatus`, `DevSimcardNumber`, `DevSimcardType`) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')"

#define UPDATEDEVICESETTING		"UPDATE `device` SET `DevName`='%s',`DevCity`='%s',`DevLocation`='%s' WHERE `DevSerialNumber`='%s'"

#define UPDATECITYNAME			"UPDATE `device` SET `DevCity`='%s' WHERE `DevCity`='%s'"
#define UPDATELOCATIONNAME		"UPDATE `device` SET `DevLocation`='%s', `DevCity`='%s' WHERE `DevCity`='%s' and `DevLocation`='%s'"
#define UPDATEDEVICENAME		"UPDATE `device` SET `DevName`='%s', `DevLocation`='%s', `DevCity`='%s' WHERE `DevSerialNumber`='%s'"

#define UPDATEDEVICE			"UPDATE `device` SET `DevSocketNumber`='%s',`DevIP`='%s',`DevPort`='%s',`DevStatus`='%s',`DevSimcardNumber`='%s',`DevSimcardType`='%s' WHERE `DevSerialNumber`='%s'"

#define SELECTUSER				"SELECT * FROM `users` WHERE `UserName`='%s' and `Password` = '%s'"
#define SELECTUSERWITHNAME		"SELECT * FROM `users` WHERE `UserName`='%s'"
#define SELECTALLUSERS			"SELECT * FROM `users`"
#define INSERTUSER				"INSERT INTO `users`(`UserName`, `Password`, `UserType`, `UserFirstName`, `UserLastName`) VALUES ('%s','%s','%s','%s','%s')"
#define UPDATEUSER				"UPDATE `users` SET `UserName`='%s',`Password`='%s',`UserType`='%s',`UserFirstName`='%s',`UserLastName`='%s'"
#define DELETEUSER				"DELETE FROM `users` WHERE `UserID`=%s"

#define INSERTUSERDEVICE		"INSERT INTO `userdevice`(`UserId`, `DeviceId`, `ViewAct`, `UpdateAct`, `DeleteAct`) VALUES (%s,%s,%s,%s,%s)"
#define SELECTUSERDEVICE		"SELECT * FROM `userdevice` WHERE `UserId` = %s and `DeviceId` = %s"
#define SELECTUSERDEVICELIST	"SELECT * FROM `userdevice` WHERE `UserId` = %s"
#define DELETEUSERDEVICE		"DELETE FROM `userdevice` WHERE `UserId`=%s and `DeviceId`=%s"

#define INSERTLOGSTATUS			"INSERT INTO `logstatus`(`DevSerialNumber`, `StatusData`, `LogDate`) VALUES ('%s','%s','%s')"
#define SELECTLOGSTATUS			"SELECT `LogID`, `DevSerialNumber`, `StatusData`, `LogDate` FROM `logstatus` WHERE `DevSerialNumber` = '%s' and `LogDate` BETWEEN '%s' and '%s'"

#define SELECTDEVICESTATUSLOG "SELECT `logstatus`.*, `device`.* FROM `logstatus` LEFT JOIN `device` ON `logstatus`.`DevSerialNumber` = `device`.`DevSerialNumber` WHERE `logstatus`.`LogDate` BETWEEN '%s' AND '%s' AND `logstatus`.`StatusData` REGEXP '%s' AND `device`.`DevSerialNumber` IN(%s)"

// Patterns for regular expression
#define DATETIMEFORMAT "^(?ni:(?=\d)((?'year'((1[6-9])|([2-9]\d))\d\d)(?'sep'[/.-])(?'month'0?[1-9]|1[012])\2(?'day'((?<!(\2((0?[2469])|11)\2))31)|(?<!\2(0?2)\2)(29|30)|((?<=((1[6-9]|[2-9]\d)(0[48]|[2468][048]|[13579][26])|(16|[2468][048]|[3579][26])00)\2\3\2)29)|((0?[1-9])|(1\d)|(2[0-8])))(?:(?=\x20\d)\x20|$))?((?<time>((0?[1-9]|1[012])(:[0-5]\d){0,2}(\x20[AP]M))|([01]\d|2[0-3])(:[0-5]\d){1,2}))?)$"

// Request Errors
#define DEVICEPERMISSIONMESSAGE	"You dont have any permission to this device"
#define DEVICEPERMISSIONNUMBER	5100

#define DEVICEEXISTMESSAGE	"Unknown device"
#define DEVICEEXISTNUMBER	5110

#define USERAUTHENTICATIONMESSAGE	"Unknown user"
#define USERAUTHENTICATIONNUMBER	5120

#define DEVICECONNECTIONMESSAGE	"Error in connect to device"
#define DEVICECONNECTIONNUMBER	5130

#define UNKNOWNRESULTMESSAGE	"Unknown result received from device"
#define UNKNOWNRESULTNUMBER		5140

#define DATABASEERRORTMESSAGE	"Unknown result received from databse"
#define DATABASEERRORNUMBER		5150

#define NODEVICEFOUNDMESSAGE	"No device found for this user"
#define NODEVICEFOUNDNUMBER		5160

#define NOCONNECTEDMESSAGE		"No Connected"
#define NOCONNECTEDNUMBER		5170

#define NOCITYLOCATIONMESSAGE	"Could not get device city and location list"
#define NOCITYLOCATIONNUMBER	5180

#define UPDATECITYMESSAGE		"Could not update city name"
#define UPDATECITYNUMBER		5190

#define UPDATELOCATIONMESSAGE	"Could not update location name"
#define UPDATELOCATIONNUMBER	5200

#define UPDATEDEVICENAMEMESSAGE	"Could not update device name"
#define UPDATEDEVICENAMENUMBER	5210

#define NOITEMFOUNDMESSAGE		"No Item found"
#define NOITEMFOUNDNUMBER		5220

#define USERPERMISSIONMESSAGE	"You dont have any permission to do this operation"
#define USERPERMISSIONNUMBER	5230

#define NOUSERFOUNDMESSAGE		"No user found"
#define NOUSERFOUNDNUMBER		5240

#define NORECORDFOUNDMESSAGE		"No record found"
#define NORECORDFOUNDNUMBER		5250



/*
* We do not implement alternate representations. However, we always
* check whether a given modifier is allowed for a certain conversion.
*/
#define ALT_E          0x01
#define ALT_O          0x02
//#define LEGAL_ALT(x)       { if (alt_format & ~(x)) return (0); }
#define LEGAL_ALT(x)       { ; }
#define TM_YEAR_BASE   (1970)

static const char *day[7] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
	"Friday", "Saturday"
};
static const char *abday[7] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
static const char *mon[12] = {
	"January", "February", "March", "April", "May", "June", "July",
	"August", "September", "October", "November", "December"
};
static const char *abmon[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
static const char *am_pm[2] = {
	"AM", "PM"
};

#endif	// _HEADER_H_

