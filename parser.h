#pragma once
#include "lexer.h"
#include "conshow.h"
#include "word.h"

class Parser
{
public:
	Lexer &lex;
	Conshow con;
	Parser(Lexer &lex) :lex(lex) {};
	~Parser() {};
	Word cur;
	void work()
	{
		Word eof("EOF", Undefined);
		cur = lex.getWord();
		while (!cur.isEOF())
		{
			statement();
		}
	}
	void var()
	{
		expect(Identifier);
		var_rest1();
	}
	void var_rest1()
	{
		if (cur.type == OLBrack)
		{
			expect(OLBrack);
			bool_exp();
			expect(ORBrack);
			var_rest1();
		}
	}
	void statement()
	{
		if (cur.type == Identifier)
		{
			var();
			expect(OEqu);
			bool_exp();
			expect(OSemi);
		}
		else if (cur.type == KIf)
		{
			cur = lex.getWord();
			expect(OLPara);
			bool_exp();
			expect(ORPara);
			statement();
			expect(KElse);
			statement();
		}
		else if (cur.type == KWhile)
		{
			cur = lex.getWord();
			expect(OLPara);
			bool_exp();
			expect(ORPara);
			statement();
		}
		else
		{
			unexpectSolve();
		}
	}
	void bool_exp()
	{
		equality();
	}
	void equality()
	{
		rel();
		equ_rest4();
	}
	void equ_rest4()
	{
		if (cur.type == OEquEqu)
		{
			expect(OEquEqu);
			rel();
			equ_rest4();
		}
		else if (cur.type == ONotEqu)
		{
			expect(ONotEqu);
			rel();
			equ_rest4();
		}
	}
	void rel()
	{
		expr();
		if (cur.type == OLess)
		{
			expect(OLess);
		}
		else if (cur.type == OLessEqu)
		{
			expect(OLessEqu);
		}
		else if (cur.type == OMore)
		{
			expect(OMore);
		}
		else if (cur.type == OMoreEqu)
		{
			expect(OMoreEqu);
		}
		else
		{
			return;
		}
		expr();
	}
	void expr()
	{
		term();
		rest5();
	}
	void rest5()
	{
		if (cur.type == OPlus)
		{
			expect(OPlus);
		}
		else if (cur.type == OMinus)
		{
			expect(OMinus);
		}
		else
		{
			return;
		}
		term();
		rest5();
	}
	void term()
	{
		unary();
		rest6();
	}
	void rest6()
	{
		if (cur.type == OMul)
		{
			expect(OMul);
		}
		else if (cur.type == ODiv)
		{
			expect(ODiv);
		}
		else
		{
			return;
		}
		unary();
		rest6();
	}
	void unary()
	{
		factor();
	}
	void factor()
	{
		if (cur.type == OLPara)
		{
			expect(OLPara);
			bool_exp();
			expect(ORPara);
		}
		else if (cur.type == Identifier)
		{
			var();
		}
		else if (cur.type == CIntDec)
		{
			expect(CIntDec);
		}
		else
		{
			unexpectSolve();
		}
	}
private:
	void expect(WordType type)
	{
		if (cur.type != type)
		{
			char tmp[128] = "Expect ";
			strcat_s(tmp, TypeName[type]);
			con.showError(tmp);
		}
		else
		{
			cur = lex.getWord();
		}
	}
	void unexpectSolve()
	{
		con.showError("Unexpected word.");
		con.showInfo(cur.raw.c_str());
		cur = lex.getWord();
	}
};

