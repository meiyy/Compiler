#pragma once
#include<cstdio>
#include<exception>
#include"conshow.h"
#include"Reader.h"
#include"Word.h"
#include<unordered_set>
using namespace std;


class KeyWordChecker
{
	const char* str[20] = {
		"int","double","char","struct","static",
		"switch","case","default","break","const",
		"return","void","continue","do","while",
		"if","else","for","sizeof","goto"
	};
public:
	unordered_set<string> hset;
	KeyWordChecker()
	{
		for (int i = 0; i < 20; i++)
		{
			hset.insert(string(str[i]));
		}
	}
};
class Parser
{
private:
	Reader &rd;
	KeyWordChecker ck;
	bool isLAlpha(char c) { return c >= 'A' && c <= 'Z'; }
	bool isSAlpha(char c) { return c >= 'a' && c <= 'z'; }
	bool isAlpha(char c) { return isLAlpha(c) || isSAlpha(c); }
	bool isNDigit(char c) { return c >= '1' && c <= '9'; }
	bool isDigit(char c) { return c >= '0' && c <= '9'; }
	bool isHex(char c) { return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
	bool isOct(char c) { return c >= '0' && c <= '7'; }
	bool isFID(char c) { return c == '_' || isAlpha(c); }
	bool isID(char c) { return isFID(c) || isDigit(c); }
	bool isBlank(char c) { return c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v'; }
	bool isKeyword(string str)
	{
		return ck.hset.find(str) != ck.hset.end();
	}
public:
	Parser(Reader *reader) :rd(*reader)
	{
	}
	
	void test()
	{
		Conshow con;
		while (rd.cur() != EOF)
		{
			while (isBlank(rd.cur()))
			{
				rd.next();
			}
			rd.pop();
			if (rd.cur() == EOF)
			{
				return;
			}
			Word word = getWord();
			string tmp = "<< " + string(TypeName[word.type]) + "\t:" + word.raw + " >>";
			con.show(tmp.c_str());
		}
	}
	
	Word getWord()
	{
		string res;
		WordType type=Undefined;
		if (rd.cur() == '\'')
		{
			rd.next();
			while (rd.cur() != '\'')
			{
				if (rd.cur() == '\\')
				{
					rd.next();
				}
				rd.next();
			}
			rd.next();
			res = rd.pop();
			type = Ccharacter;
		}
		else if (rd.cur() == '\"')
		{
			rd.next();
			while (rd.cur() != '\"')
			{
				if (rd.cur() == '\\')
				{
					rd.next();
				}
				rd.next();
			}
			rd.next();
			res = rd.pop();
			type = Cstring;
		}
		else if (rd.cur() == '#')
		{
			rd.next();
			while (rd.cur() != '\n' && rd.cur() != '\r')
			{
				if (rd.cur() == '\\')
				{
					rd.next();
				}
				rd.next();
			}
			res = rd.pop();
			type = Precompile;
		}
		else if (isFID(rd.cur()))
		{
			res = getID();
			if (isKeyword(res))
			{
				type = Keyword;
			}
			else
			{
				type = Identification;
			}
		}
		else if (rd.cur() == '0')
		{
			rd.next();
			if (rd.cur() == 'x' || rd.cur() == 'X')
			{
				rd.next();
				if (!isHex(rd.cur()))
				{
					string tmp = string("Unexpected character: ") + rd.cur() + " .";
					throw exception(tmp.c_str());
				}
				rd.next();
				res = getHex();
				type = CintHex;
			}
			else
			{
				res = getOct();
				type = CintOct;
			}
		}
		else if (isNDigit(rd.cur()))
		{
			rd.next();
			while (isDigit(rd.cur()))
			{
				rd.next();
			}
			if (rd.cur() == '.')
			{
				rd.next();
				res = get8();
				type = Cfloat;
			}
			else if (rd.cur() == 'e' || rd.cur() == 'E')
			{
				rd.next();
				res = get9();
				type = Cfloat;
			}
			else
			{
				res = rd.pop();
				type = CintDec;
			}
		}
		else if (rd.cur() == '.')
		{
			rd.next();
			if (isDigit(rd.cur()))
			{
				rd.next();
				res = get8();
				type = Cfloat;
			}
			else
			{
				res = rd.pop();
				type = Operator;
			}
		}
		else
		{
			res = getOpt();
			type = Operator;
		}
		return Word(res, type);
	}

	string getOpt()
	{
		if (rd.cur() == '+')
		{
			rd.next();
			if (rd.cur() == '=' || rd.cur() == '+')
			{
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '-')
		{
			rd.next();
			if (rd.cur() == '=' || rd.cur() == '-')
			{
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '&')
		{
			rd.next();
			if (rd.cur() == '=' || rd.cur() == '&')
			{
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '|')
		{
			rd.next();
			if (rd.cur() == '=' || rd.cur() == '|')
			{
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '<')
		{
			rd.next();
			if (rd.cur() == '<')
			{
				rd.next();
				if (rd.cur() == '=')
				{
					rd.next();
				}
				return rd.pop();
			}
			if (rd.cur() == '=')
			{
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '>')
		{
			rd.next();
			if (rd.cur() == '>')
			{
				rd.next();
				if (rd.cur() == '=')
				{
					rd.next();
				}
				return rd.pop();
			}
			if (rd.cur() == '=')
			{
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '^')
		{
			rd.next();
			if (rd.cur() == '=')
			{
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '!')
		{
			rd.next();
			if (rd.cur() == '=')
			{
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '*')
		{
			rd.next();
			if (rd.cur() == '=')
			{
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '%')
		{
			rd.next();
			if (rd.cur() == '=')
			{
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '=')
		{
			rd.next();
			if (rd.cur() == '=')
			{
				rd.next();
			}
			return rd.pop();
		}
		else
		{
			switch (rd.cur())
			{
			case '(':
			case ')':
			case '[':
			case ']':
			case '{':
			case '}':
			case '?':
			case ':':
			case ',':
			case ';':
			case '~':
				rd.next();
				return rd.pop();
			default:
				string tmp = string("Unexpected character: ") + rd.cur() + " .";
				throw exception(tmp.c_str());
			}
		}
	}

	string get8()
	{
		while (isDigit(rd.cur()))
		{
			rd.next();
		}
		if (rd.cur() == 'e' || rd.cur() == 'E')
		{
			rd.next();
			return get9();
		}
		else
		{
			return rd.pop();
		}
	}

	string get9()
	{
		if (isDigit(rd.cur()))
		{
			rd.next();
			while (isDigit(rd.cur()))
			{
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '-' || rd.cur() == '+')
		{
			rd.next();
			if (isDigit(rd.cur()))
			{
				rd.next();
				while (isDigit(rd.cur()))
				{
					rd.next();
				}
				return rd.pop();
			}
			else
			{
				string tmp = string("Unexpected character: ") + rd.cur() + " .";
				throw exception(tmp.c_str());
			}
		}
		else
		{
			string tmp = string("Unexpected character: ") + rd.cur() + " .";
			throw exception(tmp.c_str());
		}
	}

	string getID()
	{
		while (isID(rd.cur()))
		{
			rd.next();													
		}
		return rd.pop();
	}

	string getHex()
	{
		while (isHex(rd.cur()))
		{
			rd.next();
		}
		return rd.pop();
	}

	string getOct()
	{
		while (isOct(rd.cur()))
		{
			rd.next();
		}
		return rd.pop();
	}

	~Parser()
	{
	}
};

