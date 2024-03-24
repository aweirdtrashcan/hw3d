#pragma once

#include <string>

class Log
{
	friend class App;
public:
	Log();
	~Log();

	static void Debug(const std::string& message, const Log* log = nullptr);
	static void Debug(const std::wstring& message, const Log* log = nullptr);
private:
	FILE* narrow_stream = nullptr;
	char* char_stream = nullptr;
};