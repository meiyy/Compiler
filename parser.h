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
		vector<int>  falselist;
		vector<int>  truelist;
		string op;
		int quad;
		int offset;
		int place;
		int array;
		int ndim;
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
		//con.code << "j,-,-,0" << endl;
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
				//con.code << "=," << outexpr.place << ",-," << outloc.place << endl;
			}
			else
			{
				//con.code << "[]=" << outexpr.place << ",-," << outloc.place << "[" << outloc.offset << "]" << endl;
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
			ParaList outm1=m();
			ParaList outrel=rel();
			expect(ORPara);
			ParaList outm2=m();
			ParaList outstmt=stmt();
			//backpatch(outstmt.nextlist, outm1.quad);
			//backpatch(outrel.truelist, outm2.quad);
			out.nextlist = outrel.falselist;
			//emit(¡®j, -, -, ¡¯ m1.quad);
		}
		else
		{
			unexpectSolve();
		}
		return out;
	}
	ParaList loc()
	{
		ParaList out;
		expect(Identifier);
		if (cur.type == OLBrack)
		{
			con.info<<"loc --> elist]"<<endl;
			cur = lex.getWord();
			ParaList outelist=elist();
			expect(ORBrack);
			out.place = newtemp();
			//emit(¡® - , ¡¯ outelist.arry ¡®, ¡¯ C ¡®, ¡¯ out.place);
			out.offset = newtemp();
			//emit(¡®*, ¡¯ w ¡®, ¡¯ outelist.place ¡®, ¡¯ out.offset);
		}
		else
		{
			//out.place = pre.place;
			out.offset = -1;
			con.info<<"loc --> id"<<endl;
		}
		return out;
	}
	ParaList elist()
	{
		ParaList out,inrest;
		con.info<<"elist --> id[expr rest1"<<endl;
		ParaList outexpr=expr();
		//inrest.array = pre.place;
		inrest.ndim = 1;
		inrest.place = outexpr.place;
		ParaList outrest1=rest1(inrest);
		out.array = outrest1.array;
		out.ndim = outrest1.ndim;
		out.place = outrest1.place;
		return out;
	}
	ParaList rest1(ParaList in)
	{
		ParaList out;
		if (cur.type != ORBrace)
		{
			con.info<<"rest1 --> e"<<endl;
			out.array = in.array;
			out.ndim = in.ndim;
			out.place = in.place;
		}
		else
		{
			ParaList inrest1;
			con.info<<"rest1 --> ][expr rest1"<<endl;
			expect(ORBrack);
			expect(OLBrack);
			ParaList outexpr=expr();
			int t = newtemp();
			int m = inrest1.ndim + 1;
			//emit(¡®*, ¡¯ rest1.in_place ¡®, ¡¯ limit(rest1.in_array, m) ¡®, ¡¯ t);
			//emit(¡®+,¡¯ t ¡®,¡¯ expr.place ¡®,¡¯ t);
			inrest1.array = in.array;
			inrest1.ndim = m;
			inrest1.place = t;
			ParaList outrest1=rest1(inrest1);
			out.array = outrest1.array;
			out.ndim = outrest1.ndim;
			out.place = outrest1.place;
		}
		return out;
	}
	/*START--- To Be Completed */
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
		}
		rel();
		rest4();

	}
	/*END--- To Be Completed */
	ParaList rel()
	{
		ParaList out;
		con.info<<"rel --> expr relop expr"<<endl;
		ParaList outexpr1 = expr();
		ParaList outrelop = relop();
		ParaList outexpr2 = expr();
		out.truelist = makelist(nextquad());
		out.falselist = makelist(nextquad() + 1);
		//emit(¡®j¡¯ relop.op ¡®, ¡¯ expr1.place ¡®, ¡¯ expr2.place ¡®, ¡¯ ¡®0¡¯);
		//emit( ¡®j, -, -, 0¡¯ )}
		return out;
	}
	ParaList relop()
	{
		ParaList out;
		if (cur.type == OLess)
		{
			out.op = "<";
			con.info<<"relop --> <"<<endl;
			expect(OLess);
		}
		else if (cur.type == OLessEqu)
		{
			out.op = "<=";
			con.info<<"relop --> <="<<endl;
			expect(OLessEqu);
		}
		else if (cur.type == OMore)
		{
			out.op = ">";
			con.info<<"relop --> >"<<endl;
			expect(OMore);
		}
		else if (cur.type == OMoreEqu)
		{
			out.op = ">=";
			con.info<<"relop --> >="<<endl;
			expect(OMoreEqu);
		}
		else
		{
			expr();
			out.truelist = makelist(nextquad());
			out.falselist = makelist(nextquad() + 1);
			//emit(¡®jnz, ¡¯ expr.place ¡®, -, ¡¯ 0);
			//emit(¡®j, -, -, 0¡¯);}
		}
		return out;
	}
	ParaList expr()
	{
		ParaList out,inrest5;
		con.info<<"expr --> term rest5"<<endl;
		ParaList outterm=term();
		inrest5.place = outterm.place;
		ParaList outrest5=rest5(inrest5);
		out.place = outrest5.place;
		return out;
	}
	ParaList rest5(ParaList in)
	{
		ParaList out;
		if (cur.type == OPlus)
		{
			ParaList inrest5;
			con.info<<"rest5 --> +term rest5"<<endl;
			expect(OPlus);
			ParaList outterm=term();
			inrest5.place = newtemp();
			//emit(¡® + , ¡¯ rest5.in ¡®, ¡¯ term.place ¡®, ¡¯ rest51.in)
			ParaList outrest5=rest5(inrest5);
			out.place = outrest5.place;
		}
		else if (cur.type == OMinus)
		{
			ParaList inrest5;
			con.info << "rest5 --> -term rest5" << endl;
			expect(OMinus);
			ParaList outterm = term();
			inrest5.place = newtemp();
			//emit(¡® - , ¡¯ rest5.in ¡®, ¡¯ term.place ¡®, ¡¯ rest51.in)
			ParaList outrest5 = rest5(inrest5);
			out.place = outrest5.place;
		}
		else
		{
			con.info<<"rest5 --> e"<<endl;
			out.place = in.place;
		}
		return out;
	}
	ParaList term()
	{
		ParaList out,inrest6;
		con.info<<"term --> unary rest6"<<endl;
		ParaList outunary=unary();
		inrest6.place = outunary.place;
		ParaList outrest6=rest6(inrest6);
		out.place = outrest6.place;
		return out;
	}
	ParaList rest6(ParaList in)
	{
		ParaList out;
		if (cur.type == OMul)
		{
			ParaList inrest6;
			con.info<<"rest6 --> *unary rest6"<<endl;
			expect(OMul);
			ParaList outunary=unary();
			inrest6.place = newtemp();
			//emit(¡®*, ¡¯ rest6.in ¡®, ¡¯ unary.place ¡®, ¡¯ rest61.in)
			ParaList outrest6=rest6(inrest6);
			out.place = outrest6.place;
		}
		else if (cur.type == ODiv)
		{
			ParaList inrest6;
			con.info<<"rest6 --> /unary rest6"<<endl;
			expect(ODiv);
			ParaList outunary = unary();
			inrest6.place = newtemp();
			//emit(¡®/, ¡¯ rest6.in ¡®, ¡¯ unary.place ¡®, ¡¯ rest61.in)
			ParaList outrest6 = rest6(inrest6);
			out.place = outrest6.place;
		}
		else
		{
			con.info<<"rest6 --> e"<<endl;
			out.place = in.place;
		}
		return out;
	}
	ParaList unary()
	{
		ParaList out;
		con.info<<"unary --> factor"<<endl;
		ParaList outfactor=factor();
		out.place = outfactor.place;
		return out;
	}
	ParaList factor()
	{
		ParaList out;
		if (cur.type == OLPara)
		{
			con.info<<"factor --> (expr)"<<endl;
			expect(OLPara);
			ParaList outexpr=expr();
			expect(ORPara);
			out.place = outexpr.place;
		}
		else if (cur.type == Identifier)
		{
			con.info<<"factor --> loc"<<endl;
			ParaList outloc=loc();
			if (outloc.offset == -1)
			{
				out.place = outloc.place;
			}
			else
			{
				out.place = newtemp();
				//emit(¡®=[],¡¯ loc.place ¡®[¡¯ loc.offset ¡®]¡¯ ¡®, -,¡¯ factor.place );
			}
		}
		else if (cur.type == CInt)
		{
			con.info<<"factor --> num"<<endl;
			expect(CInt);
			out.place = *(int*)pre.val;
		}
		else
		{
			unexpectSolve();
		}
		return out;
	}
private:
	int nextquad() { return 0; }
	vector<int> makelist() { return vector<int>(); }
	vector<int> makelist(int n) { return vector<int>(1,n); }
	int newtemp() { return 0; }
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

