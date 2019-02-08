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

