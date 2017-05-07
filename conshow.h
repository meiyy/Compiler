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
		show("Also Another Compiler 1.0.0");
		show("BY Zeyu Shen");
		show("Compile time:" __DATE__  "  " __TIME__);
	}
	void showMenu()
	{
		cout << "Usage: Compiler.exe                 Show this help information." << endl;
		cout << "       Compiler.exe FILENAME        Process the source file." << endl;
	}
	~Conshow()
	{
	}
};

