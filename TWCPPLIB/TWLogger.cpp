#include "pch.h"
#include "TWLogger.h"
#include "TWUtils.h"

TW::TWLogger::TWLogger(const TWT::String& LogName) :
	file_based(false), filename(""), logname(LogName)
{
}

TW::TWLogger::TWLogger(const TWT::String& Filename, const TWT::String& LogName) :
	file_based(!Filename.IsEmpty()), filename(Filename), logname(LogName)
{

}

TW::TWLogger::~TWLogger() = default;

void TW::TWLogger::Log(char LogType, const TWT::String& Data) {
	tm t = TWU::GetTime();
	char time_buffer[256];
	strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", &t);

	TWT::String nlogname = !logname.IsEmpty() ? "["s + logname + "]"s : ""s;

	synchronized(sync_mutex) {
		if (file_based) {
			std::ofstream file(filename.GetData(), std::ios_base::out | std::ios_base::app);
			file << ("["s + time_buffer + "] "s + nlogname + " ["s + LogType + "] "s + Data) << '\n';
			file.close();
		} else {
			TWU::CPrintln(nlogname + " ["s + LogType + "] "s + Data);
		}
	}
}

void TW::TWLogger::LogInfo(TWT::String const& Data) {
	Log('I', Data);
}

void TW::TWLogger::LogError(TWT::String const& Data) {
	Log('E', Data);

#ifdef _DEBUG
	auto d = Data.ToCharArray();
	throw std::runtime_error(d);
#endif // _DEBUG
}