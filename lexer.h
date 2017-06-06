#pragma once
#include<cstdio>
#include<cmath>
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
class Lexer
{
private:
	Output con;
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

	void escapeLine()
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
		string tmp= string("Unexpected character: ") + rd.cur() + " .";
		throw exception(tmp.c_str());
	}
	int getEscape()
	{
		if (rd.cur() != '\\')
		{
			return -1;
		}
		rd.next();
		if (rd.cur() == '\r')
		{
			rd.next();
			if (rd.cur() == '\n')
			{
				rd.next();
			}
			return '\n';
		}
		if (rd.cur() == '\n')
		{
			rd.next();
			return '\n';
		}
		if (rd.cur() == '\'' || rd.cur() == '\"' || rd.cur() == '\?' || rd.cur() == '\\' ||
			rd.cur() == 'a' || rd.cur() == 'b' || rd.cur() == 'f' || rd.cur() == 'n' || rd.cur() == 'r' || rd.cur() == 't' || rd.cur() == 'v')
		{
			int pre = rd.cur();
			rd.next();
			return pre;
		}
		int val = 0;
		if (rd.cur() == 'x' || rd.cur() == 'X')
		{
			rd.next();
			if (!isHex(rd.cur()))
			{
				string tmp=string("Charactor ") + rd.cur() + " is illegal in hexual number.";
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
			return val;
		}
		if (isOct(rd.cur()))
		{
			rd.next();
			if (isOct(rd.cur()))
			{
				while (isOct(rd.cur()))
				{
					val = (rd.cur() - 'a' + 10) + val * 8;
					rd.next();
					if (val > 255)
					{
						string tmp = string("The char value must be less than 255.") + rd.cur() + " .";
						throw exception(tmp.c_str());
					}
				}
				// Deal with the val...
				return val;
			}
		}
		string tmp = string("Unexpected character: ") + rd.cur() + " .";
		throw exception(tmp.c_str());
		return -1;
	}
	string getOperator(WordType &type)
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
		else if (rd.cur() == '/')
		{
			rd.next();
			type = ODiv;
			if (rd.cur() == '=')
			{
				type = ODivEqu;
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
	string getID()
	{
		while (isID(rd.cur()))
		{
			rd.next();
		}
		return rd.pop();
	}
	Word _getWord()
	{
		if (rd.cur() == '\'')
		{
			rd.next();
			int val;
			if (rd.cur() == '\\')
			{
				val = getEscape();
			}
			else
			{
				val = rd.cur();
				rd.next();
			}
			if (rd.cur() != '\'')
			{
				string tmp = string("Too many characters.");
				throw exception(tmp.c_str());
			}
			rd.next();
			return Word(rd.pop(), CCharacter, (char)val);
		}
		else if (rd.cur() == '\"')
		{
			rd.next();
			while (rd.cur() != '\"')
			{
				getEscape();
			}
			rd.next();
			string val = rd.pop();
			return Word(val, CString, val);
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
			return Word(rd.pop(), Precompile);
		}
		else if (isFID(rd.cur()))
		{
			string tmp = getID();
			return Word(tmp, (WordType)ck.Check(tmp));
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
				long long val = 0;
				while (isHex(rd.cur()))
				{
					val = val * 16 + isDigit(rd.cur()) ? rd.cur() - '0' : 10 + (isLAlpha(rd.cur()) ? rd.cur() - 'a' : rd.cur() - 'A');
					rd.next();
				}
				if (val & 0xffffffff00000000ull)
				{
					throw exception("The constant is too large for int type.");
				}
				return Word(rd.pop(), CInt, (int)val);
			}
			else
			{
				int val = 0;
				while (isOct(rd.cur()))
				{
					val = val * 8 + rd.cur() - '0';
					rd.next();
				}
				return Word(rd.pop(), CInt, val);
			}
		}
		else if (isNDigit(rd.cur()))
		{
			long long val1 = 0;
			int exp = 0;
			while (isDigit(rd.cur()))
			{
				val1 = val1 * 10 + rd.cur() - '0';
				rd.next();
			}
			if (rd.cur() == '.')
			{
				rd.next();
				double val2 = 0, div = 10;
				while (isDigit(rd.cur()))
				{
					val2 = val2 + (rd.cur() - '0') / div;
					div = div / 10;
					rd.next();
				}
				if (rd.cur() == 'e' || rd.cur() == 'E')
				{
					rd.next();
					if (isDigit(rd.cur()))
					{
						exp = rd.cur() - '0';
						rd.next();
						while (isDigit(rd.cur()))
						{
							exp = exp * 10 + rd.cur() - '0';
							rd.next();
						}
					}
					else if (rd.cur() == '-' || rd.cur() == '+')
					{
						bool flag = rd.cur() == '-';
						rd.next();
						if (isDigit(rd.cur()))
						{
							exp = rd.cur() - '0';
							rd.next();
							while (isDigit(rd.cur()))
							{
								exp = exp * 10 + rd.cur() - '0';
								rd.next();
							}
							if (flag)exp = -exp;
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
				return Word(rd.pop(), CFloat, (val1 + val2)*std::exp(exp));
			}
			else if (rd.cur() == 'e' || rd.cur() == 'E')
			{
				rd.next();
				if (isDigit(rd.cur()))
				{
					exp = rd.cur() - '0';
					rd.next();
					while (isDigit(rd.cur()))
					{
						exp = exp * 10 + rd.cur() - '0';
						rd.next();
					}
				}
				else if (rd.cur() == '-' || rd.cur() == '+')
				{
					bool flag = rd.cur() == '-';
					rd.next();
					if (isDigit(rd.cur()))
					{
						exp = rd.cur() - '0';
						rd.next();
						while (isDigit(rd.cur()))
						{
							exp = exp * 10 + rd.cur() - '0';
							rd.next();
						}
						if (flag)exp = -exp;
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
				return Word(rd.pop(), CFloat, val1*std::exp(exp));
			}
			else
			{
				if (val1 & 0xffffffff00000000ull)
				{
					throw exception("The constant is too large for int type.");
				}
				return Word(rd.pop(), CInt, (int)val1);
			}
		}
		else if (rd.cur() == '.')
		{
			rd.next();
			double val2 = 0, tt = 10;
			int exp = 0;
			if (isDigit(rd.cur()))
			{
				val2 = (rd.cur() - '0') / tt;
				rd.next();
				while (isDigit(rd.cur()))
				{
					val2 = val2 + (rd.cur() - '0') / tt;
					tt = tt * 10;
					rd.next();
				}
				if (rd.cur() == 'e' || rd.cur() == 'E')
				{
					rd.next();
					if (isDigit(rd.cur()))
					{
						exp = rd.cur() - '0';
						rd.next();
						while (isDigit(rd.cur()))
						{
							exp = exp * 10 + rd.cur() - '0';
							rd.next();
						}
					}
					else if (rd.cur() == '-' || rd.cur() == '+')
					{
						bool flag = rd.cur() == '-';
						rd.next();
						if (isDigit(rd.cur()))
						{
							exp = rd.cur() - '0';
							rd.next();
							while (isDigit(rd.cur()))
							{
								exp = exp * 10 + rd.cur() - '0';
								rd.next();
							}
							if (flag)
								exp = -exp;
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
				return Word(rd.pop(), CFloat, val2*std::exp(exp));
			}
			else
			{
				return Word(rd.pop(), OPoint);
			}
		}
		else
		{
			WordType type;
			string res = getOperator(type);
			return Word(res, type);
		}
	}
	void deal()
	{
		while (true)
		{
			switch (rd.cur())
			{
			case EOF:
			case '\r':
			case '\n':
			case ' ':
			case '[':
			case ']':
			case '(':
			case ')':
			case '{':
			case '}':
			case ',':
			case ';':
				return;
			}
			rd.next();
		}
	}
public:
	Lexer(Reader *reader) :rd(*reader)
	{
	}
	
	void test()
	{
		while (rd.cur() != EOF)
		{
			escapeLine();
			while (isBlank(rd.cur()))
			{
				rd.next();
				escapeLine();
			}
			rd.pop();
			if (rd.cur() == EOF)
			{
				return;
			}
			Word word = _getWord();
			string tmp = "<< " + string(TypeName[word.type]) + "\t:" + word.raw + " >>";
			printf("<<%15s : %15s>>\n", TypeName[word.type], word.raw.c_str());
		}
	}

	Word getWord()
	{
		Word ret;
		if (rd.cur() != EOF)
		{
			try {
				escapeLine();
				while (isBlank(rd.cur()))
				{
					rd.next();
					escapeLine();
				}
				rd.pop();
				if (rd.cur() == EOF)
				{
					return Word("EOF", Undefined);
				}
				ret = _getWord();
			}
			catch(exception &e)
			{
				con.error << e.what() << endl;
				deal();
				return getWord();
			}
			return ret;
		}
		return Word("EOF",Undefined);
	}
	~Lexer()
	{
	}
};

