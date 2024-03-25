#pragma once

#include <string>
#include <vector>

class Log
{
	friend class App;
public:
	Log();
	~Log();

	static void Debug(std::string_view message, const Log* log = nullptr);
	static void Debug(std::wstring_view message, const Log* log = nullptr);
private:
	FILE* narrow_stream = nullptr;
	char* char_stream = nullptr;
};