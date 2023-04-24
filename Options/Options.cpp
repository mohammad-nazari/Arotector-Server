#include "Options.h"


Options::Options(void)
{
}


Options::~Options(void)
{
}

//************************************
// Method:    ReadOptions
// FullName:  Options::ReadOptions
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: ns__ServerOptions & ServerOptions
//
// Descriptions: return settings from settings XML file
//************************************
bool Options::ReadOptions(ns__ServerOptions& ServerOptions)
{
	// Create and initialize soap
	// gSOAP runtime context
	struct soap soap;

	// initialize runtime context 
	soap_init(&soap);

	// Set input mode
	soap_imode(&soap, SOAP_ENC_XML);

	// reset deserializers; start new (de)serialization phase 
	soap_begin(&soap);

	//
	// Load XML (De serialize)
	//

	std::ifstream fstreamIN(this->OptionsFileName);
	soap.is = &fstreamIN;

	// calls soap_begin_recv, soap_get__gt__Library and soap_end_recv
	if(soap_read_ns__ServerOptions(&soap, &ServerOptions) != SOAP_OK)
	{
		//std::cout << "soap_read failed" << std::endl;
		this->DestroySoap(&soap);

		return 0;
	}

	fstreamIN.close();

	this->DestroySoap(&soap);

	return 1;
}

//************************************
// Method:    SaveOptions
// FullName:  Options::SaveOptions
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: ns__ServerOptions & ServerOptions
//
// Description: Save setting in settings XML file
//************************************
bool Options::SaveOptions(ns__ServerOptions& ServerOptions)
{
	// Create and initialize soap
	// gSOAP runtime context
	struct soap soap;

	// initialize runtime context 
	soap_init(&soap);

	// Set input mode
	soap_imode(&soap, SOAP_ENC_XML);

	// reset deserializers; start new (de)serialization phase 
	soap_begin(&soap);

	//
	// Serialize and Save to file
	//

	soap_set_omode(&soap, SOAP_XML_INDENT);

	std::ofstream fstreamOUT(this->OptionsFileName);
	soap.os = &fstreamOUT;

	// calls soap_begin_send, soap_serialize, soap_put and soap_end_send
	if(soap_write_ns__ServerOptions(&soap, &ServerOptions) != SOAP_OK)
	{
		//ServerOptions << "soap_write failed" << std::endl;
		this->DestroySoap(&soap);

		return 0;
	}

	fstreamOUT.close();

	this->DestroySoap(&soap);

	return 1;
}


//************************************
// Method:    DestroySoap
// FullName:  Options::DestroySoap
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: struct soap * pSoap
//
// Description: Terminate and destroy soap
//************************************
void Options::DestroySoap(struct soap* pSoap)
{
	// remove deserialized class instances (C++ objects) 
	soap_destroy(pSoap);

	// clean up and remove deserialized data 
	soap_end(pSoap);

	// detach context (last use and no longer in scope)
	soap_done(pSoap);
}