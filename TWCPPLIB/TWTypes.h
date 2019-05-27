#pragma once

using namespace std::string_literals;

namespace TWT {
	using byte  = unsigned char;
	using wchar = wchar_t;

	using int8  = int8_t;
	using int16 = int16_t;
	using int64 = int64_t;

	using uint8  = uint8_t;
	using uint16 = uint16_t;
	using uint   = uint32_t;
	using uint64 = uint64_t;

	struct WString;

	struct String {
	public:
		String() = default;
		String(const String &Str) = default;
		String(const char* Data);
		String(const std::string& Data);
		~String()                         = default;

		template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		String(T Data) {
			this->data = std::to_string(Data);
		};

		const std::string& GetData() const;
		const char* ToCharArray() const;
		const TWT::uint64 GetSize() const;
		const bool IsEmpty() const;
		WString Wide();

		String& operator  = (const String& t) = default;
		String& operator += (const String& t);

	private:
		std::string data;
	};

	struct WString {
	public:
		WString() = default;
		WString(const WString &Str) = default;
		WString(const wchar* Data);
		WString(const std::wstring& Data);
		~WString() = default;

		template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		WString(T Data) {
			this->data = std::to_wstring(Data);
		};

		const std::wstring& GetData() const;
		const TWT::wchar* ToWCharArray() const;
		const uint64 GetSize() const;
		const bool IsEmpty() const;
		String Multibyte();

		WString& operator  = (const WString& Str) = default;
		WString& operator += (const WString& Str);

	private:
		std::wstring data;
	};

	template<class T>
	inline void Copy(const std::vector<T>& Source, std::vector<T>& Destination) {
		std::copy(Source.begin(), Source.end(), std::back_inserter(Destination));
	}
}  // namespace TWT

//std::ostream&  operator << (std::ostream& os, const TWT::String& t);
//std::wostream& operator << (std::wostream& os, const TWT::WString& t);

//template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
//std::string operator+(const std::string& str, const T& t) {
//	return str.data + std::to_string(t);
//}
//template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
//std::string operator+(const T& t, const std::string& str) {
//	return std::to_string(t) + str;
//}

//TWT::String  operator+(const TWT::String&  t, const TWT::String&  str);
//std::string  operator+(const std::string&  str, const TWT::String&  t);
//std::string  operator+(const TWT::String&  t, const std::string&  str);
//TWT::WString operator+(const TWT::WString& t, const TWT::WString& str);
//std::wstring operator+(const std::wstring& str, const TWT::WString& t);
//std::wstring operator+(const TWT::WString& t, const std::wstring& str);

std::ostream& operator << (std::ostream& os, const TWT::String& t);
TWT::String   operator  + (const TWT::String& t, const TWT::String& str);
std::string   operator  + (const std::string& str, const TWT::String& t);
std::string   operator  + (const TWT::String& t, const std::string& str);

std::wostream& operator << (std::wostream& os, const TWT::WString& t);
TWT::WString   operator  + (const TWT::WString& t, const TWT::WString& str);
std::wstring   operator  + (const std::wstring& str, const TWT::WString& t);
std::wstring   operator  + (const TWT::WString& t, const std::wstring& str);