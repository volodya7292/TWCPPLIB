#include "pch.h"
#include "TWUtils.h"

void TWU::CPrint(const TWT::String& str) {
	std::cout << str;
}

void TWU::CPrint(const TWT::WString& str) {
	std::wcout << str;
}

void TWU::CPrintln(const TWT::String& str) {
	std::cout << str << std::endl;
}

void TWU::CPrintln(const TWT::WString& str) {
	std::wcout << str;
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