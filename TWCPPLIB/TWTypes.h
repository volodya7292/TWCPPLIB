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
		String(String const& Str) = default;
		String(const char* Data);
		String(std::string const& Data);
		~String()                         = default;

		template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		String(T Data) {
			this->data = std::to_string(Data);
		};

		const std::string& GetData() const;
		const char* ToCharArray() const;
		const TWT::uint64 GetSize() const;
		const bool IsEmpty() const;
		WString Wide() const;

		String& operator  = (String const& t) = default;
		String& operator += (String const& t);
		bool    operator == (String const& other) const;

	private:
		std::string data;
	};

	struct WString {
	public:
		WString() = default;
		WString(WString const& Str) = default;
		WString(const wchar* Data);
		WString(std::wstring const& Data);
		~WString() = default;

		template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		WString(T Data) {
			this->data = std::to_wstring(Data);
		};

		const std::wstring& GetData() const;
		const TWT::wchar* ToWCharArray() const;
		const uint64 GetSize() const;
		const bool IsEmpty() const;
		String Multibyte() const;

		WString& operator  = (WString const& Str) = default;
		WString& operator += (WString const& Str);
		bool     operator == (WString const& other) const;

	private:
		std::wstring data;
	};

	template<class T>
	inline void Copy(std::vector<T> const& Source, std::vector<T>& Destination) {
		std::copy(Source.begin(), Source.end(), std::back_inserter(Destination));
	}

	template<typename T>
	inline T Min(T V0, T V1) {
		return glm::min(V0, V1);
	}

	template<typename T>
	inline T Max(T V0, T V1) {
		return glm::max(V0, V1);
	}
}  // namespace TWT


std::ostream& operator << (std::ostream& os, TWT::String const& t);
TWT::String   operator  + (TWT::String const& t, TWT::String const& str);
std::string   operator  + (std::string const& str, TWT::String const& t);
std::string   operator  + (TWT::String const& t, std::string const& str);

std::wostream& operator << (std::wostream const& os, TWT::WString const& t);
TWT::WString   operator  + (TWT::WString const& t, TWT::WString const& str);
std::wstring   operator  + (std::wstring const& str, TWT::WString const& t);
std::wstring   operator  + (TWT::WString const& t, std::wstring const& str);


namespace std {
	template <>
	struct hash<TWT::String> {
		std::size_t operator() (TWT::String const& str) const {
			std::hash<std::string> hash_fn;
			return hash_fn(str.GetData());
		}
	};

	template <>
	struct hash<TWT::WString> {
		std::size_t operator() (TWT::WString const& str) const {
			std::hash<std::wstring> hash_fn;
			return hash_fn(str.GetData());
		}
	};
}  // namespace std