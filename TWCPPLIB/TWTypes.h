#pragma once

using namespace std::string_literals;

namespace TWT {
	using byte  = unsigned char;
	using wchar = wchar_t;

	using int8  = int8_t;
	using int16 = int16_t;
	using int64 = int64_t;

	using uint8   = uint8_t;
	using uint16  = uint16_t;
	using uint    = uint32_t;
	using uint64 = uint64_t;

	using float64 = double;

	template<class T>
	using Vector = std::vector<T>;

	struct WString;

	struct String {
		String() = default;
	
		template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		String(T data) {
			this->data = std::to_string(data);
		};

		String(const char* data);
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
		WString(int data);
		WString(const wchar* data);
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

	char*   WideToMultibyte(const std::wstring &wstr);
	std::wstring MultibyteToWide(const std::string &str);

	template<class T>
	inline void Copy(const Vector<T>& Source, Vector<T>& Destination) {
		std::copy(Source.begin(), Source.end(), std::back_inserter(Destination));
	}
}

std::ostream&  operator << (std::ostream& os, const TWT::String& t);
std::wostream& operator << (std::wostream& os, const TWT::WString& t);

//template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
//std::string operator+(const std::string& str, const T& t) {
//	return str.data + std::to_string(t);
//}
//template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
//std::string operator+(const T& t, const std::string& str) {
//	return std::to_string(t) + str;
//}

TWT::String  operator+(const TWT::String&  t, const TWT::String&  str);
std::string  operator+(const std::string&  str, const TWT::String&  t);
std::string  operator+(const TWT::String&  t, const std::string&  str);
TWT::WString operator+(const TWT::WString& t, const TWT::WString& str);
std::wstring operator+(const std::wstring& str, const TWT::WString& t);
std::wstring operator+(const TWT::WString& t, const std::wstring& str);