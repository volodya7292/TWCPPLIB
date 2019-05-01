#pragma once
#include "TWTypes.h"

namespace TW {
	class TWLogger {
	public:
		TWLogger(const TWT::String& LogName);
		TWLogger(const TWT::String& Filename, const TWT::String& LogName);
		~TWLogger();

		void Log(TWT::Char LogType, const TWT::String& Data);
		void LogInfo(const TWT::String& Data);
		void LogError(const TWT::String& Data);

	private:
		std::mutex sync_mutex;
		const TWT::Bool file_based;
		const TWT::String filename;
		const TWT::String logname;
	};
}