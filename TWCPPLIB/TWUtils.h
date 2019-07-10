#pragma once
#include "TWTypes.h"

namespace TWU {
	template<typename T>
	void CPrint(const T& data) {
		std::cout << data;
	}

	template<typename T>
	void CPrintln(const T& data) {
		std::cout << data << std::endl;
	}

	TWT::WString HResultToWString(HRESULT Result);

	void FileExistsAssert(TWT::String const& filename);
	void FileExistsAssert(TWT::WString const& filename);

	// Read entire file into byte array
	TWT::byte* ReadFileBytes(TWT::String const& filename, TWT::uint& size);

	tm GetTime();
	double GetTimeSeconds();

	inline TWT::uint8 Log2(TWT::uint64 value) {
		unsigned long mssb; // most significant set bit
		unsigned long lssb; // least significant set bit

		if (_BitScanReverse64(&mssb, value) > 0 && _BitScanForward64(&lssb, value) > 0)
			return uint8_t(mssb + (mssb == lssb ? 0 : 1));

		return 0;
	}

	template <typename T>
	inline T AlignPowerOfTwo(T value) {
		return value == 0 ? 0 : 1 << Log2(value);
	}

	inline TWT::String BoolStr(bool Value) {
		return Value ? "True"s : "False"s;
	}
}  // namespace TWU

// ------------------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------- Synchronized macro -----------------------------------------------------
class SyncObj {
public:
	SyncObj(std::mutex &mutex) : m_mutex(mutex), m_locked(true) {
		mutex.lock();
	}

	~SyncObj() {
		m_mutex.unlock();
	}

	operator bool () const {
		return m_locked;
	}

	void unlock() {
		m_locked = false;
	}

private:
	std::mutex &m_mutex;
	bool m_locked;
};

#define synchronized(M)  for(SyncObj M##_lock = M; M##_lock; M##_lock.unlock())
// ----------------------------------------------------- Synchronized macro -----------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------

#define fsizeof(v) (sizeof(v) / sizeof(float))
#define foffsetof(s,m) (offsetof(s,m) / sizeof(float))


#define instanceof(Base, ClassDerived) dynamic_cast<ClassDerived*>(Base)        // Check if derived class relate to base class
#define readonly(Type, Name) private: Type _##Name; public: Type const& Name    // Create read-only public class member

#define prop_readonly(Type, Name) \
    private: \
        Type _##Name; \
        inline void Name##_setter(Type const& value) { } \
    public: \
        inline Type const& Name##_getter() const { return _##Name; } \
		__declspec(property(get=Name##_getter, put=Name##_setter)) Type Name;

#define prop_readwrite(Type, Name, WriteMethod) \
    private: \
        Type _##Name; \
    public: \
        inline Type const& Name##_getter() const { return _##Name; } \
		__declspec(property(get=Name##_getter, put=WriteMethod)) Type Name;