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

	template<class _Ty, size_t _Size>
	using Array = std::array<_Ty, _Size>;
	
	struct WString;

	struct String {
		String() = default;
		String(Int data);
		String(const Char* data);
		String(std::string /*data*/);
		String(const String &str) = default;
		~String() = default;

		WString Wide();

		String& operator + (const String& t);
		String& operator = (const String &t);
		String& operator += (const String& t);
		
		std::string data;
	};

	struct WString {
		WString() = default;
		WString(Int data);
		WString(const WChar* data);
		WString(std::wstring /*data*/);
		WString(const WString &str) = default;
		~WString() = default;

		String Multibyte();

		WString& operator + (const WString& t);
		WString& operator = (const WString &t);
		WString& operator += (const WString& t);

		std::wstring data;
	};
}

std::ostream& operator << (std::ostream& os, const TWT::String& t);
std::wostream& operator << (std::wostream& os, const TWT::WString& t);

template <typename T>
std::string operator+(const std::string& str, const T& t) {
	return str + std::to_string(t);
}
template <typename T>
std::string operator+(const T& t, const std::string& str) {
	return std::to_string(t) + str;
}