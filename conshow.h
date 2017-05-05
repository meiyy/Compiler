#pragma once
#include<ctime>
#include<cstring>
#include<cstdio>
#include<iostream>
#include<string>
using namespace std;
class Conshow
{
public:

	Conshow()
	{
	}

	void showError(const char* msg)
	{
		std::cerr << "[ERROR]" << msg << std::endl;
	}
	void showWarning(const char* msg)
	{
		std::cerr << "[WARNING]" << msg << std::endl;
	}
	void showInfo(const char* msg)
	{
		std::cout << "[INFO]" << msg << std::endl;
	}
	void show(const char* msg)
	{
		std::cout << msg << std::endl;
	}
	void showVersion()
	{
		time_t now = time(NULL);
		tm *now_tm = new tm();
		localtime_s(now_tm, &now);
		char now_str[100];
		sprintf_s(now_str, "Compile time:%d-%02d-%02d %02d:%02d:%02d", now_tm->tm_year + 1900, now_tm->tm_mon + 1, now_tm->tm_mday, now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
		show("Also Another Compiler 1.0.0");
		show("BY Zeyu Shen");
		show(now_str);
	}
	void showMenu()
	{
		cout << "Usage: parser                 Show this help information." << endl;
		cout << "       parser FILENAME        Process the source file." << endl;
	}
	~Conshow()
	{
	}
};

