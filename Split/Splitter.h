#pragma once

#ifndef _SPLITTER_H_
#define _SPLITTER_H_
#include <string>
#include <vector>
#include <iostream>

//! Maintains a collection of substrings that are
//! delimited by a string of one or more characters
class Splitter {
	//! Contains the split tokens
	std::vector<std::string> _tokens;
public:
	//! Subscript type for use with operator[]
	typedef std::vector<std::string>::size_type size_type;
public:

	__declspec(dllexport)Splitter ( const std::string& src, const std::string& delim );

	__declspec(dllexport)std::string& operator[] ( size_type i );

	__declspec(dllexport)const size_type size();

	__declspec(dllexport)void reset ( const std::string& src, const std::string& delim );
};

class WSplitter {
	//! Contains the split tokens
	std::vector<std::wstring> _wtokens;
public:
	//! Subscript type for use with operator[]
	typedef std::vector<std::wstring>::size_type wsize_type;
public:

	__declspec(dllexport)WSplitter ( const std::wstring& src, const std::wstring& delim );

	__declspec(dllexport)std::wstring& operator[] ( wsize_type i );

	__declspec(dllexport)const wsize_type wsize();

	__declspec(dllexport)void wreset ( const std::wstring& src, const std::wstring& delim );
};
#endif