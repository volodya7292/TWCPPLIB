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
