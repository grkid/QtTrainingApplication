#pragma once
#include <QString>
#include <string>
#include <strstream>
#define TR(a) LanguageAdapter::translate(a)

class LanguageAdapter
{
public:
	static QString translate(const char* str)
	{
		return QString::fromLocal8Bit(str);
	}

	static QString translate(const std::string str)
	{
		return QString::fromLocal8Bit(str.c_str());
	}
};

