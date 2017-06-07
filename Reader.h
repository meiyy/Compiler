#pragma once
#include<cstdio>
#include<string>
#include<exception>
using std::string;
using std::exception;
class Endoffile : public exception
{
public:
	Endoffile(const char* str) :exception(str) {}
};
class Reader
{
private:
	FILE *file;
	char *buf;
	int pos;
	int sz;
	int start;
	int lineNum;
public:
	Reader(const char *path)
	{
		fopen_s(&file, path, "rb");
		if (file == nullptr)
		{
			string res = string("File not found or not available: \"") + path + "\".";
			throw exception(res.c_str());
		}
		fseek(file, 0, SEEK_END);
		lineNum = 0;
		sz = ftell(file);
		rewind(file);

		buf = new char[sz];
		if (buf == nullptr)
		{
			throw exception("Memory error: Bad alloc.");
		}
		int readSz = fread(buf, 1, sz, file);
		if (readSz != sz)
		{
			string res = string("Error in reading file: \"") + path + "\".";
			throw exception(res.c_str());
		}
		if(file != nullptr)
			fclose(file);
		file = nullptr;
		pos = 0;
		start = 0;
	}

	void next()
	{
		if (cur() == '\n')
		{
			lineNum++;
		}
		if (pos < sz)
			pos++;
		else
			throw Endoffile("Reach the end of input file.");
	}

	int getLineNum()
	{
		return lineNum;
	}

	char cur()
	{
		if (pos >= sz)
		{
			return EOF;
		}
		return buf[pos];
	}

	string pop()
	{
		int len = pos - start;
		string res = string(buf + start, len);
		start = pos;
		return res;
	}

	~Reader()
	{
		if (file != nullptr)
			fclose(file);
		file = nullptr;
	}
};