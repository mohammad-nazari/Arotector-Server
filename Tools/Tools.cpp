#include "Tools.h"

Tools::Tools(void)
{

}


Tools::~Tools(void)
{
}


// trim from start
std::string Tools::ltrim(std::string const &str)
{
	std::string s = str;
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
std::string Tools::rtrim(std::string const &str)
{
	std::string s = str;
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
std::string Tools::trim(std::string const &str)
{
	std::string s = str;
	return ltrim(rtrim(s));
}

void Tools::replaceAll(std::string& str, const std::string& oldStr, const std::string& newStr)
{
	size_t pos = 0;
	while((pos = str.find(oldStr, pos)) != std::string::npos)
	{
		str.replace(pos, oldStr.length(), newStr);
		pos += newStr.length();
	}
}

size_t Tools::replaceOne(std::string& str, const std::string& oldStr, const std::string& newStr, size_t position)
{
	position = str.find(oldStr, position);
	if(position != std::string::npos)
	{
		str.replace(position, oldStr.length(), newStr);
		position += newStr.length();
	}
	return position;
}

size_t Tools::findOne(std::string str, const std::string& findStr, size_t position)
{
	position = str.find(findStr, position);
	if(position != std::string::npos)
	{
		return true;
	}
	return false;
}

std::string Tools::getCurrentDateTime()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "%Y-%m-%d %I:%M:%S", timeinfo);
	std::string str(buffer);

	return str;
}

std::string Tools::toUpper(std::string& str)
{
	std::string result;

	replaceOne(str, "\r\n", "", 0);

	for(unsigned int i = 0; i < str.length(); ++i)
	{
		result += toupper(str.at(i));
	}

	return result;
}

std::string Tools::toLower(std::string& str)
{
	std::string result;

	replaceOne(str, "\r\n", "", 0);

	for (unsigned int i = 0; i < str.length(); ++i)
	{
		result += tolower(str.at(i));
	}

	return result;
}

bool Tools::is_number(const std::string& str)
{
	std::string s = this->trim(str);

	if(s[0] == '-' || s[0] == '+')
	{
		s = s.substr(1);
	}
	std::string::const_iterator it = s.begin();
	while(it != s.end() && isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

bool Tools::is_numberWithLen(const std::string& str, int lenght)
{
	std::string s = this->trim(str);
	if(s.size() == lenght)
	{
		std::string::const_iterator it = s.begin();
		while(it != s.end() && isdigit(*it)) ++it;
		return !s.empty() && it == s.end();
	}
	return false;
}

bool Tools::is_double(std::string const& str)
{
	std::string s = this->trim(str);
	std::istringstream ss(s);
	double d;
	return (ss >> d) && (ss >> std::ws).eof();
}

int Tools::toInteger(std::string const& Input)
{
	int tmp = std::stoi(this->trim(Input));

	return tmp;
}

long long int Tools::toLongLong(std::string const& Input)
{
	long long int tmp = std::stoll(this->trim(Input));

	return tmp;
}

std::string Tools::toString(int Input)
{
	std::string str = std::to_string(Input);

	return str;
}

std::string Tools::toString(int Input, int Length)
{
	std::stringstream ss;
	ss << std::setw(Length) << std::setfill('0') << Input;
	std::string str = ss.str();

	return str;
}

std::string Tools::toString(long long int Input)
{
	std::string str = std::to_string(Input);

	return str;
}

std::string Tools::toString(long long int Input, int Length)
{
	std::stringstream ss;
	ss << std::setw(Length) << std::setfill('0') << Input;
	std::string str = ss.str();

	return str;
}

int Tools::getDigitLenth(long int InputNumber)
{
	int nDigits = floor(log10(abs(InputNumber))) + 1;

	return nDigits;
}

bool Tools::checkFormat(std::string Input, std::string DataFormat)
{
	std::regex regObject(DataFormat);
	bool result = regex_match(Input, regObject);

	return result;
}

bool Tools::isInString(std::string Input, std::string ParentString)
{
	bool result = ParentString.find(Input);

	return result;
}

std::string Tools::SimCardTypeToString(ns__SimcardType SimCardType)
{
	return EnumString<ns__SimcardType>::From(SimCardType);
}

bool Tools::SimCardTypeFromString(ns__SimcardType &SimcardType, std::string SimCardTypeName)
{
	bool result = EnumString<ns__SimcardType>::To(SimcardType, SimCardTypeName);
	return result;
}

std::string Tools::UserTypeToString(ns__UserType UserType)
{
	return EnumString<ns__UserType>::From(UserType);
}

bool Tools::UserTypeFromString(ns__UserType &UserType, std::string UserTypeName)
{
	bool result = EnumString<ns__UserType>::To(UserType, UserTypeName);
	return result;
}

std::string Tools::SensorTypeToString(ns__SensorType SensorType)
{
	return EnumString<ns__SensorType>::From(SensorType);
}

bool Tools::SensorTypeFromString(ns__SensorType &SensorType, std::string SensorTypeName)
{
	bool result = EnumString<ns__SensorType>::To(SensorType, SensorTypeName);
	return result;
}

std::string Tools::SensorNameToString(ns__SensorName SensorName)
{
	return EnumString<ns__SensorName>::From(SensorName);
}

bool Tools::SensorNameFromString(ns__SensorName &SensorName, std::string SensorNameName)
{
	bool result = EnumString<ns__SensorName>::To(SensorName, SensorNameName);
	return result;
}

struct tm Tools::toTMStruct(std::string Input)
{
	struct tm tmTemp;

	this->strptime(Input.c_str(), "%Y-%m-%d %H:%M:%S", &tmTemp);

	return tmTemp;
}

char * Tools::strptime(const char *buf, const char *fmt, struct tm *tm)
{
	tm->tm_sec = 0;		//< seconds (0 - 60)
	tm->tm_min = 0;		//< minutes (0 - 59)
	tm->tm_hour = 0;	//< hours (0 - 23)
	tm->tm_mday = 1;	//< day of month (1 - 31)
	tm->tm_mon = 1;		//< month of year (0 - 11)
	tm->tm_year = 1970;	//< year - 1900
	tm->tm_wday = 0;	//< day of week (Sunday = 0)
	tm->tm_yday = 0;	//< day of year (0 - 365)
	tm->tm_isdst = 0;	//< is summer time in effect?

	char c;
	const char *bp;
	size_t len = 0;
	int alt_format, i, split_year = 0;

	bp = buf;

	while((c = *fmt) != '\0')
	{
		/* Clear `alternate' modifier prior to new conversion. */
		alt_format = 0;

		/* Eat up white-space. */
		if(isspace(c))
		{
			while(isspace(*bp))
				bp++;

			fmt++;
			continue;
		}

		if((c = *fmt++) != '%')
			goto literal;


	again:        switch(c = *fmt++)
	{
		case '%': /* "%%" is converted to "%". */
			literal :
				if(c != *bp++)
					return (0);
			break;

			/*
			* "Alternative" modifiers. Just set the appropriate flag
			* and start over again.
			*/
		case 'E': /* "%E?" alternative conversion modifier. */
			LEGAL_ALT(0);
			alt_format |= ALT_E;
			goto again;

		case 'O': /* "%O?" alternative conversion modifier. */
			LEGAL_ALT(0);
			alt_format |= ALT_O;
			goto again;

			/*
			* "Complex" conversion rules, implemented through recursion.
			*/
		case 'c': /* Date and time, using the locale's format. */
			LEGAL_ALT(ALT_E);
			if(!(bp = strptime(bp, "%x %X", tm)))
				return (0);
			break;

		case 'D': /* The date as "%m/%d/%y". */
			LEGAL_ALT(0);
			if(!(bp = strptime(bp, "%m/%d/%y", tm)))
				return (0);
			break;

		case 'R': /* The time as "%H:%M". */
			LEGAL_ALT(0);
			if(!(bp = strptime(bp, "%H:%M", tm)))
				return (0);
			break;

		case 'r': /* The time in 12-hour clock representation. */
			LEGAL_ALT(0);
			if(!(bp = strptime(bp, "%I:%M:%S %p", tm)))
				return (0);
			break;

		case 'T': /* The time as "%H:%M:%S". */
			LEGAL_ALT(0);
			if(!(bp = strptime(bp, "%H:%M:%S", tm)))
				return (0);
			break;

		case 'X': /* The time, using the locale's format. */
			LEGAL_ALT(ALT_E);
			if(!(bp = strptime(bp, "%H:%M:%S", tm)))
				return (0);
			break;

		case 'x': /* The date, using the locale's format. */
			LEGAL_ALT(ALT_E);
			if(!(bp = strptime(bp, "%m/%d/%y", tm)))
				return (0);
			break;

			/*
			* "Elementary" conversion rules.
			*/
		case 'A': /* The day of week, using the locale's form. */
		case 'a':
			LEGAL_ALT(0);
			for(i = 0; i < 7; i++)
			{
				/* Full name. */
				len = strlen(day[i]);
				if(strncasecmp((char *)(day[i]), (char *)bp, len) == 0)
					break;

				/* Abbreviated name. */
				len = strlen(abday[i]);
				if(strncasecmp((char *)(abday[i]), (char *)bp, len) == 0)
					break;
			}

			/* Nothing matched. */
			if(i == 7)
				return (0);

			tm->tm_wday = i;
			bp += len;
			break;

		case 'B': /* The month, using the locale's form. */
		case 'b':
		case 'h':
			LEGAL_ALT(0);
			for(i = 0; i < 12; i++)
			{
				/* Full name. */

				len = strlen(mon[i]);
				if(strncasecmp((char *)(mon[i]), (char *)bp, len) == 0)
					break;

				/* Abbreviated name. */
				len = strlen(abmon[i]);
				if(strncasecmp((char *)(abmon[i]), (char *)bp, len) == 0)
					break;
			}

			/* Nothing matched. */
			if(i == 12)
				return (0);

			tm->tm_mon = i;
			bp += len;
			break;

		case 'C': /* The century number. */
			LEGAL_ALT(ALT_E);
			if(!(conv_num(&bp, &i, 0, 99)))
				return (0);

			if(split_year)
			{
				tm->tm_year = (tm->tm_year % 100) + (i * 100);
			}
			else
			{
				tm->tm_year = i * 100;
				split_year = 1;
			}
			break;

		case 'd': /* The day of month. */
		case 'e':
			LEGAL_ALT(ALT_O);
			if(!(conv_num(&bp, &tm->tm_mday, 1, 31)))
				return (0);
			break;

		case 'k': /* The hour (24-hour clock representation). */
			LEGAL_ALT(0);
			/* FALLTHROUGH */
		case 'H':
			LEGAL_ALT(ALT_O);
			if(!(conv_num(&bp, &tm->tm_hour, 0, 23)))
				return (0);
			break;

		case 'l': /* The hour (12-hour clock representation). */
			LEGAL_ALT(0);
			/* FALLTHROUGH */
		case 'I':
			LEGAL_ALT(ALT_O);
			if(!(conv_num(&bp, &tm->tm_hour, 1, 12)))
				return (0);
			if(tm->tm_hour == 12)
				tm->tm_hour = 0;
			break;

		case 'j': /* The day of year. */
			LEGAL_ALT(0);
			if(!(conv_num(&bp, &i, 1, 366)))
				return (0);
			tm->tm_yday = i - 1;
			break;

		case 'M': /* The minute. */
			LEGAL_ALT(ALT_O);
			if(!(conv_num(&bp, &tm->tm_min, 0, 59)))
				return (0);
			break;

		case 'm': /* The month. */
			LEGAL_ALT(ALT_O);
			if(!(conv_num(&bp, &i, 1, 12)))
				return (0);
			tm->tm_mon = i - 1;
			break;

			//            case 'p': /* The locale's equivalent of AM/PM. */
			//                LEGAL_ALT(0);
			//                /* AM? */
			//                if (strcasecmp(am_pm[0], bp) == 0) 
			//                {
			//                    if (tm->tm_hour > 11)
			//                        return (0);
			//
			//                    bp += strlen(am_pm[0]);
			//                    break;
			//                }
			//                /* PM? */
			//                else if (strcasecmp(am_pm[1], bp) == 0) 
			//                {
			//                    if (tm->tm_hour > 11)
			//                        return (0);
			//
			//                    tm->tm_hour += 12;
			//                    bp += strlen(am_pm[1]);
			//                    break;
			//                }
			//
			//                /* Nothing matched. */
			//                return (0);

		case 'S': /* The seconds. */
			LEGAL_ALT(ALT_O);
			if(!(conv_num(&bp, &tm->tm_sec, 0, 61)))
				return (0);
			break;

		case 'U': /* The week of year, beginning on sunday. */
		case 'W': /* The week of year, beginning on monday. */
			LEGAL_ALT(ALT_O);
			/*
			* XXX This is bogus, as we can not assume any valid
			* information present in the tm structure at this
			* point to calculate a real value, so just check the
			* range for now.
			*/
			if(!(conv_num(&bp, &i, 0, 53)))
				return (0);
			break;

		case 'w': /* The day of week, beginning on sunday. */
			LEGAL_ALT(ALT_O);
			if(!(conv_num(&bp, &tm->tm_wday, 0, 6)))
				return (0);
			break;

		case 'Y': /* The year. */
			LEGAL_ALT(ALT_E);
			if(!(conv_num(&bp, &i, 0, 9999)))
				return (0);

			tm->tm_year = i - 1900;
			break;

		case 'y': /* The year within 100 years of the epoch. */
			LEGAL_ALT(ALT_E | ALT_O);
			if(!(conv_num(&bp, &i, 0, 99)))
				return (0);

			if(split_year)
			{
				tm->tm_year = ((tm->tm_year / 100) * 100) + i;
				break;
			}
			split_year = 1;
			if(i <= 68)
				tm->tm_year = i + 2000 - TM_YEAR_BASE;
			else
				tm->tm_year = i + 1900 - TM_YEAR_BASE;
			break;

			/*
			* Miscellaneous conversions.
			*/
		case 'n': /* Any kind of white-space. */
		case 't':
			LEGAL_ALT(0);
			while(isspace(*bp))
				bp++;
			break;


		default: /* Unknown/unsupported conversion. */
			return (0);
	}


	}

	/* LINTED functional specification */
	return ((char *)bp);
}

int Tools::conv_num(const char **buf, int *dest, int llim, int ulim)
{
	int result = 0;

	/* The limit also determines the number of valid digits. */
	int rulim = ulim;

	if(**buf < '0' || **buf > '9')
		return (0);

	do
	{
		result *= 10;
		result += *(*buf)++ - '0';
		rulim /= 10;
	} while((result * 10 <= ulim) && rulim && **buf >= '0' && **buf <= '9');

	if(result < llim || result > ulim)
		return (0);

	*dest = result;
	return (1);
}

int Tools::strncasecmp(char *s1, char *s2, size_t n)
{
	if(n == 0)
		return 0;

	while(n-- != 0 && tolower(*s1) == tolower(*s2))
	{
		if(n == 0 || *s1 == '\0' || *s2 == '\0')
			break;
		s1++;
		s2++;
	}

	return tolower(*(unsigned char *)s1) - tolower(*(unsigned char *)s2);
}
