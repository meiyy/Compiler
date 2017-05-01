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
	const char* str[9] = {
		"if","else","while","int","double",
		"break","continue","const","return"
	};
public:
	int Check(string c)
	{
		for (int i = 0; i < 9; i++)
		{
			if (c == string(str[i]))
				return i;
		}
		return Identifier;
	}
	KeyWordChecker()
	{
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
			printf("<<%15s : %15s>>\n", TypeName[word.type], word.raw.c_str());
			//con.show(tmp.c_str());
		}
	}
	void getEscape()
	{
		if (rd.cur() != '\\')
		{
			return;
		}
		rd.next();
		if (rd.cur() == '\r')
		{
			rd.next();
			if (rd.cur() == '\n')
			{
				rd.next();
			}
			return;
		}
		if (rd.cur() == '\n')
		{
			rd.next();
			return;
		}
		if (rd.cur() == '\'' || rd.cur() == '\"' || rd.cur() == '\?' || rd.cur() == '\\' ||
			rd.cur() == 'a' || rd.cur() == 'b' || rd.cur() == 'f' || rd.cur() == 'n' || rd.cur() == 'r' || rd.cur() == 't' || rd.cur() == 'v')
		{
			rd.next();
			return;
		}
		if (rd.cur() == '0')
		{
			rd.next();
			int val = 0;
			if (rd.cur() == 'x' || rd.cur() == 'X')
			{
				rd.next();
				if (!isHex(rd.cur()))
				{
					string tmp = string("Unexpected character: ") + rd.cur() + " .";
					throw exception(tmp.c_str());
				}
				while (isHex(rd.cur()))
				{
					val = (rd.cur() > '9' ? (rd.cur() - 'a' + 10) : (rd.cur() - '0')) + val * 16;
					rd.next();
					if (val > 255)
					{
						string tmp = string("The char value must be less than 255.") + rd.cur() + " .";
						throw exception(tmp.c_str());
					}
				}
				// Deal with the val...
				return;
			}
			else if (isOct(rd.cur()))
			{
				while (isOct(rd.cur()))
				{
					val = (rd.cur() - 'a' + 10)  + val * 8;
					rd.next();
					if (val > 255)
					{
						string tmp = string("The char value must be less than 255.") + rd.cur() + " .";
						throw exception(tmp.c_str());
					}
				}
				// Deal with the val...
				return;
			}
		}
		string tmp = string("Unexpected character: ") + rd.cur() + " .";
		throw exception(tmp.c_str());
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
					getEscape();
				}
				rd.next();
			}
			rd.next();
			res = rd.pop();
			type = CCharacter;
		}
		else if (rd.cur() == '\"')
		{
			rd.next();
			while (rd.cur() != '\"')
			{
				getEscape();
			}
			rd.next();
			res = rd.pop();
			type = CString;
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
			type = (WordType)ck.Check(res);
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
				type = CIntHex;
			}
			else
			{
				res = getOct();
				type = CIntOct;
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
				type = CFloat;
			}
			else if (rd.cur() == 'e' || rd.cur() == 'E')
			{
				rd.next();
				res = get9();
				type = CFloat;
			}
			else
			{
				res = rd.pop();
				type = CIntDec;
			}
		}
		else if (rd.cur() == '.')
		{
			rd.next();
			if (isDigit(rd.cur()))
			{
				rd.next();
				res = get8();
				type = CFloat;
			}
			else
			{
				res = rd.pop();
				type = OPoint;
			}
		}
		else
		{
			res = getOpt(type);
		}
		return Word(res, type);
	}

	string getOpt(WordType &type)
	{
		if (rd.cur() == '+')
		{
			rd.next();
			type = OPlus;
			if (rd.cur() == '=')
			{
				type = OPlusEqu;
				rd.next();
			}
			else if (rd.cur() == '+')
			{

				type = OPlusPlus;
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '-')
		{
			rd.next();
			type = OMinus;
			if (rd.cur() == '=')
			{
				type = OMinusEqu;
				rd.next();
			}
			else if (rd.cur() == '-')
			{

				type = OMinusMinus;
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '&')
		{
			rd.next();
			type = OAnd;
			if (rd.cur() == '=')
			{
				type = OAndEqu;
				rd.next();
			}
			else if (rd.cur() == '&')
			{
				type = OAndAnd;
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '|')
		{
			rd.next();
			type = OOr;
			if (rd.cur() == '=')
			{
				type = OOrEqu;
				rd.next();
			}
			else if (rd.cur() == '|')
			{
				type = OOrOr;
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '<')
		{
			rd.next();
			type = OLess;
			if (rd.cur() == '<')
			{
				rd.next();
				type = OLShift;
				if (rd.cur() == '=')
				{
					type = OLShiftEqu;
					rd.next();
				}
				return rd.pop();
			}
			if (rd.cur() == '=')
			{
				type = OLessEqu;
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '>')
		{
			rd.next();
			type = OMore;
			if (rd.cur() == '>')
			{
				type = ORShift;
				rd.next();
				if (rd.cur() == '=')
				{
					type = ORShiftEqu;
					rd.next();
				}
				return rd.pop();
			}
			if (rd.cur() == '=')
			{
				type = OMoreEqu;
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '^')
		{
			rd.next();
			type = OXor;
			if (rd.cur() == '=')
			{
				type = OXorEqu;
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '!')
		{
			rd.next();
			type = ONot;
			if (rd.cur() == '=')
			{
				type = ONotEqu;
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '*')
		{
			rd.next();
			type = OMul;
			if (rd.cur() == '=')
			{
				type = OMulEqu;
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '%')
		{
			rd.next();
			type = OMod;
			if (rd.cur() == '=')
			{
				type = OModEqu;
				rd.next();
			}
			return rd.pop();
		}
		else if (rd.cur() == '=')
		{
			rd.next();
			type = OEqu;
			if (rd.cur() == '=')
			{
				type = OEquEqu;
				rd.next();
			}
			return rd.pop();
		}
		else
		{
			switch (rd.cur())
			{
			case '(':
				type = OLPara;
				rd.next();
				return rd.pop();
			case ')':
				type = ORPara;
				rd.next();
				return rd.pop();
			case '[':
				type = OLBrack;
				rd.next();
				return rd.pop();
			case ']':
				type = ORBrack;
				rd.next();
				return rd.pop();
			case '{':
				type = OLBrace;
				rd.next();
				return rd.pop();
			case '}':
				type = ORBrace;
				rd.next();
				return rd.pop();
			case '?':
				type = OQue;
				rd.next();
				return rd.pop();
			case ':':
				type = OColon;
				rd.next();
				return rd.pop();
			case ',':
				type = OComma;
				rd.next();
				return rd.pop();
			case ';':
				type = OSemi;
				rd.next();
				return rd.pop();
			case '~':
				type = OTilde;
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

