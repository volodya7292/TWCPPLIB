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

	void FileExistsAssert(TWT::String filename);
	void FileExistsAssert(TWT::WString filename);

	// Read entire file into byte array
	TWT::Byte* ReadFileBytes(TWT::String filename, TWT::Int& size);

	TWT::Float64 GetTime();
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
