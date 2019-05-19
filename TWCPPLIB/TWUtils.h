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

	void FileExistsAssert(TWT::String filename);
	void FileExistsAssert(TWT::WString filename);

	// Read entire file into byte array
	TWT::byte* ReadFileBytes(TWT::String filename, TWT::uint& size);

	tm GetTime();
	double GetTimeSeconds();

	inline TWT::uint8 Log2(TWT::uint64 value) {
		unsigned long mssb; // most significant set bit
		unsigned long lssb; // least significant set bit

		if (_BitScanReverse64(&mssb, value) > 0 && _BitScanForward64(&lssb, value) > 0)
			return uint8_t(mssb + (mssb == lssb ? 0 : 1));
		else
			return 0;
	}

	template <typename T>
	inline T AlignPowerOfTwo(T value) {
		return value == 0 ? 0 : 1 << Log2(value);
	}

	inline TWT::String BoolStr(bool Value) {
		return Value ? "True"s : "False"s;
	}
}

// ------------------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------- Synchronized macro -----------------------------------------------------
class Lock {
public:
	//the default constructor
	Lock(std::mutex& mutex) : m_mutex(mutex), m_locked(true) {
		mutex.lock();
	}

	//the destructor
	~Lock() {
		m_mutex.unlock();
	}

	//report the state of locking when used as a boolean
	operator bool() const {
		return m_locked;
	}

	//unlock
	void set_unlock() {
		m_locked = false;
	}

private:
	std::mutex& m_mutex;
	bool m_locked;
};
#define synchronized(M)  for(Lock M##_lock = M; M##_lock; M##_lock.set_unlock())
// ----------------------------------------------------- Synchronized macro -----------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------

#define fsizeof(v) (sizeof(v) / sizeof(float))
#define foffsetof(s,m) (offsetof(s,m) / sizeof(float))