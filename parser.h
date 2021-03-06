#pragma once
#include "lexer.h"
#include "conshow.h"
#include "word.h"
#include<tuple>
#include<vector>
#include<sstream>
typedef int PlaceType;
class Parser
{
	struct ToInt
	{
		int operator()(int num)
		{
			return num;
		}
		int operator()(string str)
		{
			int b;
			stringstream(str) >> b;
			return b;
		}
	};
	struct ToString
	{
		template<class T>
		string operator()(T something)
		{
			ostringstream str;
			str << something;
			return str.str();
		}
	};
	struct ParaList
	{
		int nextlist=-1;
		int falselist=-1;
		int truelist=-1;
		string op="Null";
		int quad=-1;
		string offset="Null";
		string place="Null";
		string array="Null";
		int ndim=-1;
	};
	struct Quad
	{
		Quad()
			:op(), arg1(), arg2(), ret() {}
		Quad(string op, string arg1, string arg2, string ret)
			:op(op), arg1(arg1), arg2(arg2), ret(ret) {}
		string op;
		string arg1;
		string arg2;
		string ret;
	};
	int lastPos = 0;
	vector<Quad> quads;
public:
	Lexer &lex;
	Output con;
	Parser(Lexer &lex) :lex(lex) {};
	~Parser() {};
	Word cur, pre;
	void work()
	{
		cur = lex.getWord();
		/*while(!cur.isEOF())
		stmt();*/

		ParaList out = stmts();
		emit("nop", "-", "-", "-");
		if(out.nextlist!=-1)
			backpatch(out.nextlist, nextquad() - 1);
		int cnt = 0;
		for (auto i : quads)
		{
			con.code << cnt << "#\t" << i.op << "\t," << i.arg1 << "\t," << i.arg2 << "\t," << i.ret << endl;
			cnt++;
		}
	}
	ParaList stmts()
	{
		ParaList out, inrest0;
		con.info << "stmts --> stmt rest0" << endl;
		ParaList outstmt = stmt();
		inrest0.nextlist = outstmt.nextlist;
		ParaList outrest0 = rest0(inrest0);
		out.nextlist = outrest0.nextlist;
		return out;
	}
	ParaList rest0(ParaList in)
	{
		ParaList out;
		if (cur.type == Identifier || cur.type == KIf || cur.type == KWhile)
		{
			ParaList inrest01;
			con.info << "rest0 --> m stmt rest0" << endl;
			ParaList outm = m();
			ParaList outstmt = stmt();
			backpatch(in.nextlist, outm.quad);
			inrest01.nextlist = outstmt.nextlist;
			ParaList outrest01 = rest0(inrest01);
			out.nextlist = outrest01.nextlist;
		}
		else
		{
			con.info << "rest0 --> e" << endl;
			out.nextlist = in.nextlist;
		}
		return out;
	}
	ParaList m()
	{
		ParaList out;
		con.info << "m --> e" << endl;
		out.quad = nextquad();
		return out;
	}
	ParaList n()
	{
		ParaList out;
		con.info << "n --> e" << endl;
		out.nextlist = nextquad();
		emit("j", "-", "-", "-1");
		return out;
	}
	ParaList stmtBlock()
	{
		ParaList out;
		if (cur.type == OLBrace)
		{
			expect(OLBrace);
			ParaList outstmts=stmts();
			expect(ORBrace);
			out.nextlist = outstmts.nextlist;
		}
		else
		{
			ParaList outstmt=stmt();
			out.nextlist = outstmt.nextlist;
		}
		return out;
	}
	ParaList stmt()
	{
		ParaList out;
		if (cur.type == Identifier)
		{
			con.info << "stmt --> loc = expr;" << endl;
			ParaList outloc = loc();
			expect(OEqu);
			ParaList outexpr = expr();
			expect(OSemi);
			if (outloc.offset == "")
			{
				emit("=", ToString()(outexpr.place), "-", ToString()(outloc.place));
			}
			else
			{
				emit("[]=", ToString()(outexpr.place), "-", ToString()(outloc.place) + "[" + ToString()(outloc.offset) + "]");
			}
			out.nextlist = -1;
		}
		else if (cur.type == KIf)
		{

			cur = lex.getWord();
			expect(OLPara);
			ParaList outrel = rel();
			expect(ORPara);
			ParaList outm1 = m();
			ParaList outstmt1 = stmtBlock();
			if (cur.type == KElse)
			{
				ParaList outn = n();
				con.info << "stmt --> if(rel)m stmt else m stmt" << endl;
				expect(KElse);
				ParaList outm2 = m();
				ParaList outstmt2 = stmtBlock();
				backpatch(outrel.truelist, outm1.quad);
				backpatch(outrel.falselist, outm2.quad);
				out.nextlist = outstmt1.nextlist;
				mergeList(out.nextlist, outn.nextlist);
				mergeList(out.nextlist, outstmt2.nextlist);
			}
			else
			{
				con.info << "stmt --> if(rel)m stmt" << endl;
				backpatch(outrel.truelist, outm1.quad);
				out.nextlist = outstmt1.nextlist;
				mergeList(out.nextlist, outrel.falselist);
			}

		}
		else if (cur.type == KWhile)
		{
			con.info << "stmt --> while(m rel)m stmt" << endl;
			cur = lex.getWord();
			expect(OLPara);
			ParaList outm1 = m();
			ParaList outrel = rel();
			expect(ORPara);
			ParaList outm2 = m();
			ParaList outstmt = stmtBlock();
			backpatch(outstmt.nextlist, outm1.quad);
			backpatch(outrel.truelist, outm2.quad);
			out.nextlist = outrel.falselist;
			emit("j", "-", "-", ToString()(outm1.quad));
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
			con.info << "loc --> elist]" << endl;
			ParaList outelist = elist();
			out.place = newtemp();
			emit("-", ToString()(outelist.array), "C", ToString()(out.place));
			out.offset = newtemp();
			emit("*", "w", ToString()(outelist.place), ToString()(out.offset));
		}
		else
		{
			out.place = pre.raw;
			out.offset = "";
			con.info << "loc --> id" << endl;
		}
		return out;
	}
	ParaList elist()
	{
		ParaList out, inrest;
		con.info << "elist --> id[expr rest1" << endl;
		inrest.array = pre.raw;
		expect(OLBrack);
		ParaList outexpr = expr();
		inrest.ndim = 1;
		inrest.place = outexpr.place;
		ParaList outrest1 = rest1(inrest);
		out.array = outrest1.array;
		out.ndim = outrest1.ndim;
		out.place = outrest1.place;
		return out;
	}
	ParaList rest1(ParaList in)
	{
		expect(ORBrack);
		ParaList out;
		if (cur.type != OLBrack)
		{
			con.info << "rest1 --> e" << endl;
			out.array = in.array;
			out.ndim = in.ndim;
			out.place = in.place;
		}
		else
		{
			ParaList inrest1;
			con.info << "rest1 --> ][expr rest1" << endl;
			expect(OLBrack);
			ParaList outexpr = expr();
			string t = newtemp();
			int m = in.ndim + 1;
			emit("*", ToString()(in.place), ToString()(limit(in.array, m)), ToString()(t));
			emit("+", ToString()(t), ToString()(outexpr.place), ToString()(t));
			inrest1.array = in.array;
			inrest1.ndim = m;
			inrest1.place = t;
			ParaList outrest1 = rest1(inrest1);
			out.array = outrest1.array;
			out.ndim = outrest1.ndim;
			out.place = outrest1.place;
		}
		return out;
	}
	/*START--- To Be Completed */
	ParaList bool_exp()
	{
		con.info << "bool --> equality" << endl;
		equality();
	}
	ParaList equality()
	{
		con.info << "equality --> rel rest4" << endl;
		rel();
		rest4();
	}
	ParaList rest4()
	{
		if (cur.type == OEquEqu)
		{
			con.info << "rest4 --> == rel rest4" << endl;
			expect(OEquEqu);
		}
		else if (cur.type == ONotEqu)
		{
			con.info << "rest4 --> != rel rest4" << endl;
			expect(ONotEqu);
		}
		else
		{
			con.info << "rest4 --> e" << endl;
		}
		rel();
		rest4();

	}
	/*END--- To Be Completed */
	ParaList rel()
	{
		ParaList out;
		con.info << "rel --> expr relop expr" << endl;
		ParaList outexpr1 = expr();
		ParaList outrelop = relop();
		ParaList outexpr2 = expr();
		out.truelist = nextquad();
		out.falselist = nextquad() + 1;
		emit("j" + outrelop.op, ToString()(outexpr1.place), ToString()(outexpr2.place), "-1");
		emit("j", "-", "-", "-1");
		return out;
	}
	ParaList relop()
	{
		ParaList out;
		if (cur.type == OLess)
		{
			out.op = "<";
			con.info << "relop --> <" << endl;
			expect(OLess);
		}
		else if (cur.type == OLessEqu)
		{
			out.op = "<=";
			con.info << "relop --> <=" << endl;
			expect(OLessEqu);
		}
		else if (cur.type == OMore)
		{
			out.op = ">";
			con.info << "relop --> >" << endl;
			expect(OMore);
		}
		else if (cur.type == OMoreEqu)
		{
			out.op = ">=";
			con.info << "relop --> >=" << endl;
			expect(OMoreEqu);
		}
		else
		{
			ParaList outexpr = expr();
			out.truelist = nextquad();
			out.falselist = nextquad() + 1;
			emit("jnz", ToString()(outexpr.place), "-", "-1");
			emit("j", "-", "-", "-1");
		}
		return out;
	}
	ParaList expr()
	{
		ParaList out, inrest5;
		con.info << "expr --> term rest5" << endl;
		ParaList outterm = term();
		inrest5.place = outterm.place;
		ParaList outrest5 = rest5(inrest5);
		out.place = outrest5.place;
		return out;
	}
	ParaList rest5(ParaList in)
	{
		ParaList out;
		if (cur.type == OPlus)
		{
			ParaList inrest5;
			con.info << "rest5 --> +term rest5" << endl;
			expect(OPlus);
			ParaList outterm = term();
			inrest5.place = newtemp();
			emit("+", ToString()(in.place), ToString()(outterm.place), ToString()(inrest5.place));
			ParaList outrest5 = rest5(inrest5);
			out.place = outrest5.place;
		}
		else if (cur.type == OMinus)
		{
			ParaList inrest5;
			con.info << "rest5 --> -term rest5" << endl;
			expect(OMinus);
			ParaList outterm = term();
			inrest5.place = newtemp();
			emit("-", ToString()(in.place), ToString()(outterm.place), ToString()(inrest5.place));
			ParaList outrest5 = rest5(inrest5);
			out.place = outrest5.place;
		}
		else
		{
			con.info << "rest5 --> e" << endl;
			out.place = in.place;
		}
		return out;
	}
	ParaList term()
	{
		ParaList out, inrest6;
		con.info << "term --> unary rest6" << endl;
		ParaList outunary = unary();
		inrest6.place = outunary.place;
		ParaList outrest6 = rest6(inrest6);
		out.place = outrest6.place;
		return out;
	}
	ParaList rest6(ParaList in)
	{
		ParaList out;
		if (cur.type == OMul)
		{
			ParaList inrest6;
			con.info << "rest6 --> *unary rest6" << endl;
			expect(OMul);
			ParaList outunary = unary();
			inrest6.place = ToString()(newtemp());
			emit("*", ToString()(in.place), ToString()(outunary.place), ToString()(inrest6.place));
			ParaList outrest6 = rest6(inrest6);
			out.place = outrest6.place;
		}
		else if (cur.type == ODiv)
		{
			ParaList inrest6;
			con.info << "rest6 --> /unary rest6" << endl;
			expect(ODiv);
			ParaList outunary = unary();
			inrest6.place = newtemp();
			emit("/", ToString()(in.place), ToString()(outunary.place), ToString()(inrest6.place));
			ParaList outrest6 = rest6(inrest6);
			out.place = outrest6.place;
		}
		else
		{
			con.info << "rest6 --> e" << endl;
			out.place = in.place;
		}
		return out;
	}
	ParaList unary()
	{
		ParaList out;
		con.info << "unary --> factor" << endl;
		ParaList outfactor = factor();
		out.place = outfactor.place;
		return out;
	}
	ParaList factor()
	{
		ParaList out;
		if (cur.type == OLPara)
		{
			con.info << "factor --> (expr)" << endl;
			expect(OLPara);
			ParaList outexpr = expr();
			expect(ORPara);
			out.place = outexpr.place;
		}
		else if (cur.type == Identifier)
		{
			con.info << "factor --> loc" << endl;
			ParaList outloc = loc();
			if (outloc.offset == "")
			{
				out.place = outloc.place;
			}
			else
			{
				out.place = newtemp();
				emit("=[]", ToString()(outloc.place) + "[" + ToString()(outloc.offset) + "]", "-", ToString()(out.place));
			}
		}
		else if (cur.type == CInt)
		{
			con.info << "factor --> num" << endl;
			expect(CInt);
			out.place = ToString()(*((int*)pre.val));
		}
		else
		{
			unexpectSolve();
		}
		return out;
	}
private:
	string limit(string arrayName, int ndim)
	{
		return "D" + ToString()(ndim);
	}
	void backpatch(int nextlist, int quadnum)
	{
		int pos = nextlist;
		while (pos!=-1)
		{
			int tmp = ToInt()(quads[pos].ret);
			quads[pos].ret = ToString()(quadnum);
			pos = tmp;
		}
	}
	int nextquad()
	{
		return lastPos;
	}
	void emit(string op, string arg1, string arg2, string ret)
	{
		while (lastPos >= (int)quads.size())
		{
			quads.push_back(Quad());
		}
		quads[lastPos] = Quad(op, arg1, arg2, ret);
		lastPos++;
	}
	void mergeList(int &to, int from)
	{
		if (from == -1)
			return;
		int pos = from;
		while (ToInt()(quads[pos].ret) != -1)
		{
			pos = ToInt()(quads[pos].ret);
		}
		quads[pos].ret = ToString()(to);
		to = from;
	}
	string newtemp()
	{
		static int cnt = 0;
		return "X" + ToString()(cnt++);
	}
	void expect(WordType type)
	{
		if (cur.type != type)
		{
			con.error <<"Line "<<cur.lineNum<<": Expect "<< TypeName[type] << endl;
		}
		else
		{
			pre = cur;
			cur = lex.getWord();
		}
	}
	void unexpectSolve()
	{
		con.error << "Line " << cur.lineNum << ": Unexpected word:" << cur.raw << endl;
		cur = lex.getWord();
	}
};