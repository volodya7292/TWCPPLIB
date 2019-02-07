#include "pch.h"
#include "TWTypes.h"

TWT::String::String(Int data) {
	this->data = std::to_string(data);
}

TWT::String::String(const Char* data) :
	data(data)
{
}

TWT::String::String(std::string data) {
	this->data = move(data);
}

TWT::WString TWT::String::Wide() {
	std::wstring stemp = std::wstring(data.begin(), data.end());
	return WString(stemp);
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

TWT::WString::WString(std::wstring data) {
	this->data = move(data);
}

TWT::String TWT::WString::Multibyte() {
	std::string stemp = std::string(data.begin(), data.end());
	return String(stemp);
}

//TWT::WString& TWT::WString::operator+(const WString& t) {
//	data += t.data;
//	return *this;
//}

TWT::WString& TWT::WString::operator+=(const WString& t) {
	data += t.data;
	return *this;
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
