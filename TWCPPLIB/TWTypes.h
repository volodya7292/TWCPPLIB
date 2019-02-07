#pragma once

using namespace std::string_literals;

namespace TWT {
	using Bool = bool;

	using Char  = char;
	using Byte  = unsigned char;
	using WChar = wchar_t;

	using Int8  = int8_t;
	using Int16 = int16_t;
	using Int   = int32_t;
	using Int64 = int64_t;

	using UInt8  = uint8_t;
	using UInt16 = uint16_t;
	using UInt   = uint32_t;
	using UInt64 = uint64_t;

	using Float   = float;
	using Float64 = double;

	template<class T>
	using Vector = std::vector<T>;

	struct WString;

	struct String {
		String() = default;
		String(Int data);
		String(const Char* data);
		String(std::string data);
		String(const String &str) = default;
		~String()                 = default;

		WString Wide();

		//String& operator + (const String& t);
		String& operator  = (const String& t) = default;
		String& operator += (const String& t);

		std::string data;
	};

	struct WString {
		WString() = default;
		WString(Int data);
		WString(const WChar* data);
		WString(std::string data);
		WString(std::wstring data);
		WString(const WString &str) = default;
		~WString() = default;

		String Multibyte();

		//WString& operator +  (const WString& t);
		WString& operator  = (const WString& t) = default;
		WString& operator += (const WString& t);

		std::wstring data;
	};

	TWT::Char*   WideToMultibyte(const std::wstring &wstr);
	std::wstring MultibyteToWide(const std::string &str);
}

std::ostream&  operator << (std::ostream& os, const TWT::String& t);
std::wostream& operator << (std::wostream& os, const TWT::WString& t);

template <class T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
std::string operator+(const std::string& str, const T& t) {
	return str.data + std::to_string(t);
}
template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
std::string operator+(const T& t, const std::string& str) {
	return std::to_string(t) + str;
}

std::string  operator+(const std::string&  str, const TWT::String&  t);
std::string  operator+(const TWT::String&  t, const std::string&  str);
std::wstring operator+(const std::wstring& str, const TWT::WString& t);
std::wstring operator+(const TWT::WString& t, const std::wstring& str);