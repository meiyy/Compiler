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
		bool isNull;
		bool st;
	public:
		Myostream(ostream& out,string prefix,bool isNull = false):isNull(isNull),out(out),prefix(prefix),st(true)
		{

		}

		
		Myostream& operator<<(int something)
		{
			if (isNull)
			{
				return *this;
			}
			if (st)out << prefix;
			out << something;
			st = false;
			return *this;
		}

		Myostream& operator<<(string something)
		{
			if (isNull)
			{
				return *this;
			}
			if (st)out << prefix;
			out << something;
			st = false;
			return *this;
		}

		Myostream& operator<<(char* something)
		{
			if (isNull)
			{
				return *this;
			}
			if (st)out << prefix;
			out << something;
			st = false;
			return *this;
		}

		Myostream& operator<<(ostream& (*endl)(ostream& strm))
		{
			if (isNull)
			{
				return *this;
			}
			endl(out);
			st = true;
			return *this;
		}
	};
public:
	Myostream out,error,info,warning,code;
	Output() :
		out(cout, ""),
		error(cerr, "[Error] "),
		info(cerr, "[Info] ",true),
		warning(cerr, "[Warning] "),
		code(clog,"[Code] ")
	{
	}
	void showVersion()
	{
		out << "Also Another Compiler 1.0.0" << endl;
		out << "BY Zeyu Shen" << endl;
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

