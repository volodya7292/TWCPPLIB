#pragma once
#include "TWTypes.h"

namespace TWU {
	void CPrint(const TWT::String& str);
	void CPrint(const TWT::WString& str);
	void CPrintln(const TWT::String& str);
	void CPrintln(const TWT::WString& str);

	void FileExistsAssert(TWT::String filename);
	void FileExistsAssert(TWT::WString filename);
}

