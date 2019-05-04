#include "pch.h"
#include "TWUtils.h"

TWT::WString TWU::HResultToWString(HRESULT Result) {
	_com_error err(Result);
	return TWT::WString(err.ErrorMessage());
}

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

TWT::Byte* TWU::ReadFileBytes(TWT::String filename, TWT::UInt& size) {
	TWU::FileExistsAssert(filename.Wide());

	std::ifstream VertexFile(filename.data, std::ios::ate | std::ios::binary);

	if (VertexFile.is_open()) {
		size = (TWT::Int)VertexFile.tellg();

		TWT::Byte* data = new TWT::Byte[size];

		VertexFile.seekg(0);
		VertexFile.read(reinterpret_cast<char*>(data), size);
		VertexFile.close();

		return data;
	}

	return nullptr;
}

TWT::Time TWU::GetTime() {
	time_t theTime = time(NULL);
	tm *aTime = localtime(&theTime);
	return *aTime;
}

TWT::Float64 TWU::GetTimeSeconds() {
	TWT::Float64 time = static_cast<TWT::Float64>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
	return time / 1.0e9;
}
