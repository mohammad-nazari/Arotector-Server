#pragma once
#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include "OptionsH.h"
#include "../ProtectorServer/Header.h"

class Options
{
public:
	__declspec(dllexport)Options(void);
	__declspec(dllexport)~Options(void);

	__declspec(dllexport)bool ReadOptions(ns__ServerOptions& ServerOptions);
	__declspec(dllexport)bool SaveOptions(ns__ServerOptions& ServerOptions);

	void DestroySoap(struct soap* pSoap);
private:
	ns__ServerOptions serverOptions;
	std::string OptionsFileName;
	CRITICAL_SECTION optionsCriticalSection;
};
#endif // End _OPTIONS_H_
