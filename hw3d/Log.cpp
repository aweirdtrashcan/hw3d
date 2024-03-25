#include "Log.h"

#include <iostream>
#include "ChiliWin.h"

#include <sstream>
#include <format>

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

void Log::Debug(std::string_view message, const Log* log)
{
#ifdef _DEBUG
	DWORD written = 0;
	std::string output = std::format("[DEBUG]: {}\n", message.data());

	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(), (DWORD)output.size(), &written, nullptr);
#endif
}

void Log::Debug(std::wstring_view message, const Log* log)
{
#ifdef _DEBUG
	DWORD written = 0;
	std::wstring output = std::format(L"[DEBUG]: {}\n", message.data());

	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(), (DWORD)output.size(), &written, nullptr);
#endif
}
