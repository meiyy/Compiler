#pragma once
#include<ctime>
#include<cstring>
#include<cstdio>
#include<iostream>
#include<string>
using namespace std;

class Output
{
	class Myostream
	{
		ostream &out;
		string prefix;
	public:
		Myostream(ostream& out,string prefix):out(out),prefix(prefix)
		{
		}

		template<class T>
		ostream& operator<<(T something)
		{
			out << prefix << something;
			return out;
		}
	};
public:
	Myostream out,error,info,warning,code;
	Output():
		out(cout, ""),
		error(cerr, "[Error] "),
		info(cerr, "[Info] "),
		warning(cerr, "[Warning] "),
		code(clog,"[Code] ")
	{
	}
	void showVersion()
	{
		out<<"Also Another Compiler 1.0.0"<<endl;
		out<<"BY Zeyu Shen" << endl;
		out << "Compile time:" __DATE__  "  " __TIME__ << endl;
	}
	void showMenu()
	{
		out << "Usage: Compiler.exe                 Show this help information." << endl;
		out << "       Compiler.exe FILENAME        Process the source file." << endl;
	}
	~Output()
	{
	}
};

