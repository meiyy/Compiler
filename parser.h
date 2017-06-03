#pragma once
#include "lexer.h"
#include "conshow.h"
#include "word.h"
#include<tuple>
#include<vector>
typedef int PlaceType;
class Parser
{
	struct ParaList
	{
		vector<int> nextlist;
		int quad;
		int falselist;
		int truelist;
		int offset;
		int place;
	};
public:
	Lexer &lex;
	Output con;
	Parser(Lexer &lex) :lex(lex) {};
	~Parser() {};
	Word cur,pre;
	void work()
	{
		cur = lex.getWord();
		/*while(!cur.isEOF())
			stmt();*/
		stmts();
	}
	void stmts()
	{
		ParaList out,inrest0;
		con.info<<"stmts --> stmt rest0"<<endl;
		ParaList outstmt=stmt();
		inrest0.nextlist = outstmt.nextlist;
		ParaList outrest0=rest0(inrest0);
		out.nextlist = outrest0.nextlist;
	}
	ParaList rest0(ParaList in)
	{
		ParaList out;
		if (cur.type == Identifier || cur.type == KIf || cur.type == KWhile)
		{
			ParaList inrest01;
			con.info<<"rest0 --> m stmt rest0"<<endl;
			ParaList outm=m();
			ParaList outstmt=stmt();
			//{backpatch(in.nextlist, outm.quad);
			inrest01.nextlist = outstmt.nextlist;
			ParaList outrest01=rest0(inrest01);
			out.nextlist = outrest01.nextlist;
		}
		else
		{
			con.info<<"rest0 --> e"<<endl;
			out.nextlist = in.nextlist;
		}
		return out;
	}
	ParaList m()
	{
		ParaList out;
		con.info<<"m --> e"<<endl;
		out.quad = nextquad();
		return out;
	}
	ParaList n()
	{
		ParaList out;
		con.info<<"n --> e"<<endl;
		out.nextlist = makelist(nextquad());
		con.code << "j,-,-,0" << endl;
		return out;
	}
	ParaList stmt()
	{
		ParaList out;
		if (cur.type == Identifier)
		{
			con.info<<"stmt --> loc = expr;"<<endl;
			ParaList outloc=loc();
			expect(OEqu);
			ParaList outexpr=expr();
			expect(OSemi);
			if (outloc.offset == -1)
			{
				con.code << "=," << outexpr.place << ",-," << outloc.place << endl;
			}
			else
			{
				con.code << "[]=" << outexpr.place << ",-," << outloc.place << "[" << outloc.offset << "]" << endl;
			}
			out.nextlist = makelist();
		}
		else if (cur.type == KIf)
		{
			con.info<<"stmt --> if(rel)m stmt else m stmt"<<endl;
			cur = lex.getWord();
			expect(OLPara);
			ParaList outrel=rel();
			expect(ORPara);
			ParaList outm1 = m();
			ParaList outstmt1=stmt();
			ParaList outn=n();
			expect(KElse);
			ParaList outm2 = m();
			ParaList outstmt2=stmt();
			//backpatch(outrel.truelist, outm1.quad);
			//backpatch(outrel.falselist, outm2.quad);
			out.nextlist = outstmt1.nextlist;
			out.nextlist.insert(out.nextlist.end(), outn.nextlist.begin(), outn.nextlist.end());
			out.nextlist.insert(out.nextlist.end(), outstmt2.nextlist.begin(), outstmt2.nextlist.end());
		}
		else if (cur.type == KWhile)
		{
			con.info<<"stmt --> while(m rel)m stmt"<<endl;
			cur = lex.getWord();
			expect(OLPara);
			m();
			rel();
			expect(ORPara);
			m();
			stmt();
		}
		else
		{
			unexpectSolve();
		}
	}
	ParaList loc()
	{
		expect(Identifier);
		if (cur.type == OLBrack)
		{
			con.info<<"loc --> elist]"<<endl;
			cur = lex.getWord();
			elist();
		}
		else
		{
			con.info<<"loc --> id"<<endl;
		}
	}
	ParaList elist()
	{
		con.info<<"elist --> id[expr rest1"<<endl;
		expr();
		rest1();
	}
	ParaList rest1()
	{
		if (cur.type != ORBrace)
		{
			con.info<<"rest1 --> e"<<endl;
			
		}
		else
		{
			con.info<<"rest1 --> ][expr rest1"<<endl;
			expect(ORBrack);
			expect(OLBrack);
			expr();
			rest1();
		}
	}
	ParaList bool_exp()
	{
		con.info<<"bool --> equality"<<endl;
		equality();
	}
	ParaList equality()
	{
		con.info<<"equality --> rel rest4"<<endl;
		rel();
		rest4();
	}
	ParaList rest4()
	{
		if (cur.type == OEquEqu)
		{
			con.info<<"rest4 --> == rel rest4"<<endl;
			expect(OEquEqu);
		}
		else if (cur.type == ONotEqu)
		{
			con.info<<"rest4 --> != rel rest4"<<endl;
			expect(ONotEqu);
		}
		else
		{
			con.info<<"rest4 --> e"<<endl;
			return;
		}
		rel();
		rest4();
	}
	ParaList rel()
	{
		con.info<<"rel --> expr relop expr"<<endl;
		expr();
		relop();
		expr();
	}
	ParaList relop()
	{
		if (cur.type == OLess)
		{
			con.info<<"relop --> <"<<endl;
			expect(OLess);
		}
		else if (cur.type == OLessEqu)
		{
			con.info<<"relop --> <="<<endl;
			expect(OLessEqu);
		}
		else if (cur.type == OMore)
		{
			con.info<<"relop --> >"<<endl;
			expect(OMore);
		}
		else if (cur.type == OMoreEqu)
		{
			con.info<<"relop --> >="<<endl;
			expect(OMoreEqu);
		}
		else
		{
			expr();
		}
	}
	ParaList expr()
	{
		con.info<<"expr --> term rest5"<<endl;
		term();
		rest5();
	}
	ParaList rest5()
	{
		if (cur.type == OPlus)
		{
			con.info<<"rest5 --> +term rest5"<<endl;
			expect(OPlus);
			term();
			rest5();
		}
		else if (cur.type == OMinus)
		{
			con.info<<"rest5 --> -term rest5"<<endl;
			expect(OMinus);
			term();
			rest5();
		}
		else
		{
			con.info<<"rest5 --> e"<<endl;
			return;
		}
	}
	ParaList term()
	{
		con.info<<"term --> unary rest6"<<endl;
		unary();
		rest6();
	}
	ParaList rest6()
	{
		if (cur.type == OMul)
		{
			con.info<<"rest6 --> *unary rest6"<<endl;
			expect(OMul);
			unary();
			rest6();
		}
		else if (cur.type == ODiv)
		{
			con.info<<"rest6 --> /unary rest6"<<endl;
			expect(ODiv);
			unary();
			rest6();
		}
		else
		{
			con.info<<"rest6 --> e"<<endl;
			return;
		}
	}
	ParaList unary()
	{
		con.info<<"unary --> factor"<<endl;
		factor();
	}
	ParaList factor()
	{
		if (cur.type == OLPara)
		{
			con.info<<"factor --> (expr)"<<endl;
			expect(OLPara);
			expr();
			expect(ORPara);
		}
		else if (cur.type == Identifier)
		{
			con.info<<"factor --> loc"<<endl;
			loc();
		}
		else if (cur.type == CInt)
		{
			con.info<<"factor --> num"<<endl;
			expect(CInt);
		}
		else
		{
			unexpectSolve();
		}
	}
private:
	int nextquad() { return 0; }
	vector<int> makelist() { return vector<int>(); }
	vector<int> makelist(int n) { return vector<int>(1,n); }
	void expect(WordType type)
	{
		if (cur.type != type)
		{
			char tmp[128] = "Expect ";
			strcat_s(tmp, TypeName[type]);
			con.error << tmp << endl;
		}
		else
		{
			pre = cur;
			cur = lex.getWord();
		}
	}
	void unexpectSolve()
	{
		con.error<<"Unexpected word:"<<cur.raw<<endl;
		cur = lex.getWord();
	}
};

