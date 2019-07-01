#include "pch.h"
#include "TWUtils.h"

TWT::WString TWU::HResultToWString(HRESULT Result) {
	_com_error err(Result);
	return TWT::WString(err.ErrorMessage());
}

void TWU::FileExistsAssert(TWT::String const& filename) {
	std::ifstream file(filename.GetData());

	if (!file.good())
		throw std::runtime_error("File "s + filename + " not good(not exists)!"s);
}

void TWU::FileExistsAssert(TWT::WString const& filename) {
	std::wifstream file(filename.GetData());

	if (!file.good())
		throw std::runtime_error("File "s + filename.Multibyte() + " not good(not exists)!"s);
}

TWT::byte* TWU::ReadFileBytes(TWT::String const& filename, TWT::uint& size) {
	TWU::FileExistsAssert(filename.Wide());

	std::ifstream file(filename.GetData(), std::ios::ate | std::ios::binary);

	if (file.is_open()) {
		size = (int)file.tellg();

		auto* data = new TWT::byte[size];

		file.seekg(0);
		file.read(reinterpret_cast<char*>(data), size);
		file.close();

		return data;
	}

	return nullptr;
}

tm TWU::GetTime() {
	time_t theTime = time(nullptr);
	tm *aTime = localtime(&theTime);
	return *aTime;
}

double TWU::GetTimeSeconds() {
	double time = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
	return time / 1.0e9;
}
