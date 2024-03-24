#include "Log.h"

#include <iostream>
#include "ChiliWin.h"

#include <sstream>

Log* _local_log = nullptr;

Log::Log()
{
	AllocConsole();
	bool failed = freopen_s(&narrow_stream, "CONOUT$", "w", stdout);

	if (failed)
	{
		throw std::exception("Failed to open out console!");
	}

	char_stream = (char*)malloc(10000);
	_local_log = this;
}

Log::~Log()
{
	_local_log = nullptr;

	fflush(narrow_stream);
	fclose(narrow_stream);
	free(char_stream);
}

void Log::Debug(const std::string& message, const Log* log)
{
#ifdef _DEBUG
	const Log* log_inst = log ? log : _local_log;

	std::stringstream oss;

	oss << "[DEBUG]: " << message << "\n";

	const std::string& str = oss.str();

	DWORD written = 0;

	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), str.c_str(), (DWORD)str.size(), &written, nullptr);
#endif
}

void Log::Debug(const std::wstring& message, const Log* log)
{
#ifdef _DEBUG
	const Log* log_inst = log ? log : _local_log;

	std::wstringstream oss;

	oss << L"[DEBUG]: " << message << L"\n";

	const std::wstring& wstr = oss.str();

	DWORD written = 0;

	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), wstr.c_str(), (DWORD)wstr.size(), &written, nullptr);
#endif
}
