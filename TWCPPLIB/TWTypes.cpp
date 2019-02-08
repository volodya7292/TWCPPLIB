#include "pch.h"
#include "TWTypes.h"

TWT::String::String(const Char* data) :
	data(data)
{
}

TWT::String::String(std::string data) {
	this->data = move(data);
}

TWT::WString TWT::String::Wide() {
	return WString(MultibyteToWide(data));
	//std::wstring stemp = std::wstring(data.begin(), data.end());
	//return WString(stemp);
}

//std::string& TWT::String::operator+(const TWT::String & t) {
//	data += t.data;
//	return data;
//}

TWT::String& TWT::String::operator+=(const String& t) {
	data += t.data;
	return *this;
}


TWT::WString::WString(Int data) {
	this->data = std::to_wstring(data);
}

TWT::WString::WString(const WChar* data) :
	data(data)
{
}

TWT::WString::WString(std::string data) {
	this->data = MultibyteToWide(data);
}

TWT::WString::WString(std::wstring data) {
	this->data = move(data);
}

TWT::String TWT::WString::Multibyte() {
	return WideToMultibyte(data);
	//std::string stemp = std::string(data.begin(), data.end());
	//return String(stemp);
}

//TWT::WString& TWT::WString::operator+(const WString& t) {
//	data += t.data;
//	return *this;
//}

TWT::WString& TWT::WString::operator+=(const WString& t) {
	data += t.data;
	return *this;
}


TWT::Char* TWT::WideToMultibyte(const std::wstring &wstr) {
	if (wstr.empty()) return new char[1];
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);

	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);

	size_t data_size = strTo.length() + 1;
	char* data = new char[data_size];
	strcpy_s(data, data_size, strTo.c_str());

	return data;
}

std::wstring TWT::MultibyteToWide(const std::string &str) {
	if (str.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}


std::ostream& operator<<(std::ostream& os, const TWT::String& t) {
	return os << t.data;
}

std::wostream& operator<<(std::wostream& os, const TWT::WString& t) {
	return os << t.data;
}

std::string operator+(const std::string & str, const TWT::String & t) {
	return str + t.data;
}

std::string operator+(const TWT::String & t, const std::string & str) {
	return t.data + str;
}

std::wstring operator+(const std::wstring & str, const TWT::WString & t) {
	return str + t.data;
}

std::wstring operator+(const TWT::WString & t, const std::wstring & str) {
	return t.data + str;
}