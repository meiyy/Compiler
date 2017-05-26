#pragma once
#include "lexer.h"
#include "conshow.h"
#include "word.h"
typedef int PlaceType;
class Parser
{
public:
	Lexer &lex;
	Conshow con;
	Parser(Lexer &lex) :lex(lex) {};
	~Parser() {};
	Word cur,pre;
	void work()
	{
		cur = lex.getWord();
		while(!cur.isEOF())
			stmt();
	}
	void stmt()
	{
		if (cur.type == Identifier)
		{
			con.showInfo("stmt --> loc = bool;");
			loc();
			expect(OEqu);
			bool_exp();
			expect(OSemi);
		}
		else if (cur.type == KIf)
		{
			con.showInfo("stmt --> if(bool)stmt else stmt");
			cur = lex.getWord();
			expect(OLPara);
			bool_exp();
			expect(ORPara);
			stmt();
			expect(KElse);
			stmt();
		}
		else if (cur.type == KWhile)
		{
			con.showInfo("stmt --> while(bool) stmt");
			cur = lex.getWord();
			expect(OLPara);
			bool_exp();
			expect(ORPara);
			stmt();
		}
		else
		{
			unexpectSolve();
		}
	}
	void loc()
	{
		con.showInfo("loc --> id rest1");
		expect(Identifier);
		rest1();
	}
	void rest1()
	{
		if (cur.type == OLBrack)
		{
			con.showInfo("rest1 --> [bool] rest1");
			expect(OLBrack);
			bool_exp();
			expect(ORBrack);
			rest1();
		}
		else
		{
			con.showInfo("rest1 --> e");
		}
	}
	void bool_exp()
	{
		con.showInfo("bool --> equality");
		equality();
	}
	void equality()
	{
		con.showInfo("equality --> rel rest4");
		rel();
		rest4();
	}
	void rest4()
	{
		if (cur.type == OEquEqu)
		{
			con.showInfo("rest4 --> == rel rest4");
			expect(OEquEqu);
		}
		else if (cur.type == ONotEqu)
		{
			con.showInfo("rest4 --> != rel rest4");
			expect(ONotEqu);
		}
		else
		{
			con.showInfo("rest4 --> e");
			return;
		}
		rel();
		rest4();
	}
	void rel()
	{
		con.showInfo("rel --> expr rop_expr");
		expr();
		rop_expr();
	}
	void rop_expr()
	{
		if (cur.type == OLess)
		{
			con.showInfo("rop_expr --> < expr");
			expect(OLess);
		}
		else if (cur.type == OLessEqu)
		{
			con.showInfo("rop_expr --> <= expr");
			expect(OLessEqu);
		}
		else if (cur.type == OMore)
		{
			con.showInfo("rop_expr --> > expr");
			expect(OMore);
		}
		else if (cur.type == OMoreEqu)
		{
			con.showInfo("rop_expr --> >= expr");
			expect(OMoreEqu);
		}
		else
		{
			con.showInfo("rop_expr --> e");
			return;
		}
		expr();
	}
	void expr()
	{
		con.showInfo("expr --> term rest5");
		term();
		rest5();
	}
	void rest5()
	{
		if (cur.type == OPlus)
		{
			con.showInfo("rest5 --> +term rest5");
			expect(OPlus);
		}
		else if (cur.type == OMinus)
		{
			con.showInfo("rest5 --> -term rest5");
			expect(OMinus);
		}
		else
		{
			con.showInfo("rest5 --> e");
			return;
		}
		term();
		rest5();
	}
	void term()
	{
		con.showInfo("term --> unary rest6");
		unary();
		rest6();
	}
	void rest6()
	{
		if (cur.type == OMul)
		{
			con.showInfo("rest6 --> *unary rest6");
			expect(OMul);
		}
		else if (cur.type == ODiv)
		{
			con.showInfo("rest6 --> /unary rest6");
			expect(ODiv);
		}
		else
		{
			con.showInfo("rest6 --> e");
			return;
		}
		unary();
		rest6();
	}
	void unary()
	{
		con.showInfo("unary --> factor");
		factor();
	}
	void factor()
	{
		if (cur.type == OLPara)
		{
			con.showInfo("factor --> (bool)");
			expect(OLPara);
			bool_exp();
			expect(ORPara);
		}
		else if (cur.type == Identifier)
		{
			con.showInfo("factor --> loc");
			loc();
		}
		else if (cur.type == CIntDec)
		{
			con.showInfo("factor --> num");
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
			pre = cur;
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

