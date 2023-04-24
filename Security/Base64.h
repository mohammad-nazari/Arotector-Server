#pragma once

#ifndef _BASE64_H_
#define _BASE64_H_

#include "../ProtectorServer/Header.h"

class Base64
{
public:
	__declspec(dllexport)Base64(void);
	__declspec(dllexport)~Base64(void);

	__declspec(dllexport)std::string Encode(std::string Input);
	__declspec(dllexport)std::string Decode(std::string Input);

protected:
private:
	std::string base64_chars;
	static inline bool is_base64( unsigned char c );
	std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
	std::string base64_decode(std::string const& encoded_string);
};

#endif // _BASE64_H_