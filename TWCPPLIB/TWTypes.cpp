#include "pch.h"
#include "TWTypes.h"

TWT::String::String(const char* Data) :
	data(Data) {
}

TWT::String::String(const std::string& Data) :
	data(Data) {
}

const std::string& TWT::String::GetData() const {
	return data;
}

const char* TWT::String::ToCharArray() const {
	const char* temp = data.c_str();
	return temp;
}

const TWT::uint64 TWT::String::GetSize() const {
	return data.size();
}

const bool TWT::String::IsEmpty() const {
	return GetSize() == 0;
}

TWT::WString TWT::String::Wide() {
	if (data.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &data[0], (int)data.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &data[0], (int)data.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

TWT::String& TWT::String::operator+=(const String& t) {
	this->data += t.data;
	return *this;
}

bool TWT::String::operator==(String const& other) const {
	return data.compare(other.data) == 0;
}

std::ostream& operator<<(std::ostream& os, const TWT::String& t) {
	return os << t.GetData();
}

TWT::String operator+(const TWT::String& t, const TWT::String& str) {
	return t.GetData() + str.GetData();
}

std::string operator+(const std::string& str, const TWT::String& t) {
	return str + t.GetData();
}

std::string operator+(const TWT::String& t, const std::string& str) {
	return t.GetData() + str;
}

// -----------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------

TWT::WString::WString(const wchar* Data) :
	data(Data) {

}

TWT::WString::WString(const std::wstring& Data) :
	data(Data) {
}

const std::wstring& TWT::WString::GetData() const {
	return data;
}

const TWT::wchar* TWT::WString::ToWCharArray() const {
	const wchar* temp = data.c_str();
	return temp;
}

const TWT::uint64 TWT::WString::GetSize() const {
	return data.size();
}

const bool TWT::WString::IsEmpty() const {
	return GetSize() == 0;
}

TWT::String TWT::WString::Multibyte() {

	if (data.empty()) return new char[1];
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &data[0], (int)data.size(), NULL, 0, NULL, NULL);

	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &data[0], (int)data.size(), &strTo[0], size_needed, NULL, NULL);

	size_t data_size = strTo.length() + 1;
	char* data = new char[data_size];
	strcpy_s(data, data_size, strTo.c_str());

	TWT::String str = data;
	delete[] data;
	return str;
}

TWT::WString& TWT::WString::operator+=(const WString& Str) {
	data += Str.data;
	return *this;
}

bool TWT::WString::operator==(WString const& other) const {
	return data.compare(other.data) == 0;
}

std::wostream& operator<<(std::wostream& os, const TWT::WString& t) {
	return os << t.GetData();
}

TWT::WString operator+(const TWT::WString& t, const TWT::WString& str) {
	return t.GetData() + str.GetData();
}

std::wstring operator+(const std::wstring& str, const TWT::WString& t) {
	return str + t.GetData();
}

std::wstring operator+(const TWT::WString& t, const std::wstring& str) {
	return t.GetData() + str;
}