#include "pch.h"
#include "TWUtils.h"

void TWU::FileExistsAssert(TWT::String filename) {
	std::ifstream file(filename.data);

	if (!file.good())
		throw std::runtime_error("File "s + filename + " not good(not exists)!"s);
}

void TWU::FileExistsAssert(TWT::WString filename) {
	std::wifstream file(filename.data);

	if (!file.good())
		throw std::runtime_error("File "s + filename.Multibyte() + " not good(not exists)!"s);
}

TWT::Float64 TWU::GetTime() {
	TWT::Float64 time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	//TWT::UInt digits = static_cast<TWT::UInt>(log10(time)) + 1;
	
	return time / 1.0e9;

}
