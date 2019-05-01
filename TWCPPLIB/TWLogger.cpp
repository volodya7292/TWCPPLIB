#include "pch.h"
#include "TWLogger.h"
#include "TWUtils.h"

TW::TWLogger::TWLogger(const TWT::String& LogName) :
	file_based(false), filename(""), logname(LogName)
{
}

TW::TWLogger::TWLogger(const TWT::String& Filename, const TWT::String& LogName) :
	file_based(Filename.data.size() > 0), filename(Filename), logname(LogName)
{

}

TW::TWLogger::~TWLogger() {

}

void TW::TWLogger::Log(TWT::Char LogType, const TWT::String& Data) {
	TWT::Time t = TWU::GetTime();
	TWT::Char time_buffer[256];
	strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", &t);

	TWT::String nlogname = logname.data.size() > 0 ? "["s + logname + "]"s : ""s;

	synchronized(sync_mutex) {
		if (file_based) {
			std::ofstream file(filename.data, std::ios_base::out | std::ios_base::app);
			file << ("["s + time_buffer + "] "s + nlogname + " ["s + LogType + "] "s + Data) << '\n';
			file.close();
		} else {
			TWU::CPrintln(nlogname + " ["s + LogType + "] "s + Data);
		}
	}
}

void TW::TWLogger::LogInfo(const TWT::String& Data) {
	Log('I', Data);
}

void TW::TWLogger::LogError(const TWT::String& Data) {
	Log('E', Data);
}
