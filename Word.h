#pragma once
#include<string>
using std::string;

enum WordType
{
	KIf,
	KElse,
	KWhile,
	KInt,
	KDouble,
	KBreak,
	KContinue,
	KConst,
	KReturn,
	Identifier,
	ODiv,
	ODivEqu,
	OTilde,
	OQue,
	OComma,
	OColon,
	OLBrace,
	ORBrace,
	OSemi,
	OEqu,
	OEquEqu,
	OMod,
	OModEqu,
	OMul,
	OMulEqu,
	ONot,
	ONotEqu,
	OXor,
	OXorEqu,
	ORShift,
	ORShiftEqu,
	OMoreEqu,
	OLShift,
	OLShiftEqu,
	OLessEqu,
	OOr,
	OOrOr,
	OOrEqu,
	OAnd,
	OAndAnd,
	OAndEqu,
	OMinusMinus,
	OMinusEqu,
	OPlusPlus,
	OPlusEqu,
	OLess,
	OMore,
	OPoint,
	OPlus,
	OMinus,
	OLPara,
	ORPara,
	OLBrack,
	ORBrack,
	CInt,
	CFloat,
	CString,
	CCharacter,
	Precompile,
	Undefined
};
const char* TypeName[60]= {
	"KIf",
	"KElse",
	"KWhile",
	"KInt",
	"KDouble",
	"KBreak",
	"KContinue",
	"KConst",
	"KReturn",
	"Identifier",
	"ODiv",
	"ODivEqu",
	"OTilde",
	"OQue",
	"OComma",
	"OColon",
	"OLBrace",
	"ORBrace",
	"OSemi",
	"OEqu",
	"OEquEqu",
	"OMod",
	"OModEqu",
	"OMul",
	"OMulEqu",
	"ONot",
	"ONotEqu",
	"OXor",
	"OXorEqu",
	"ORShift",
	"ORShiftEqu",
	"OMoreEqu",
	"OLShift",
	"OLShiftEqu",
	"OLessEqu",
	"OOr",
	"OOrOr",
	"OOrEqu",
	"OAnd",
	"OAndAnd",
	"OAndEqu",
	"OMinusMinus",
	"OMinusEqu",
	"OPlusPlus",
	"OPlusEqu",
	"OLess",
	"OMore",
	"OPoint",
	"OPlus",
	"OMinus",
	"OLPara",
	"ORPara",
	"OLBrack",
	"ORBrack",
	"CInt",
	"CFloat",
	"CString",
	"CCharacter",
	"Precompile",
	"Undefined"
};



class Word
{
public:
	string raw;
	WordType type;
	void *val;
	Word() { val = nullptr; }
	Word(Word& a)
	{
		raw = a.raw;
		type = a.type;
		if (type == CInt)val = new int(*((int*)(a.val)));
		else if (type == CFloat)val = new double(*(double*)(a.val));
		else if (type == CCharacter)val = new char(*(char*)(a.val));
		else if (type == CString)val = new string(*(string*)(a.val));
		else val = nullptr;
	}
	Word(Word&& a)
	{
		raw = a.raw;
		type = a.type;
		val = a.val;
		a.val = nullptr;
	}
	Word& operator=(Word& a)
	{
		raw = a.raw;
		type = a.type;
		if (type == CInt)val = new int(*((int*)(a.val)));
		else if (type == CFloat)val = new double(*(double*)(a.val));
		else if (type == CCharacter)val = new char(*(char*)(a.val));
		else if (type == CString)val = new string(*(string*)(a.val));
		else val = nullptr;
		return *this;
	}
	Word& operator=(Word&& a)
	{
		raw = a.raw;
		type = a.type;
		val = a.val;
		a.val = nullptr;
		return *this;
	}
	Word(string raw, WordType type) :raw(raw), type(type), val(nullptr){}
	Word(string raw, WordType type, int val) :raw(raw), type(type) { this->val = new int(val); }
	Word(string raw, WordType type, char val) :raw(raw), type(type) { this->val = new char(val); }
	Word(string raw, WordType type, double val) :raw(raw), type(type) { this->val = new double(val); }
	Word(string raw, WordType type, string val) :raw(raw), type(type) { this->val = new string(val); }

	
	bool isEOF()
	{
		return raw == "EOF" && type == Undefined;
	}

	~Word()
	{
		if (val != nullptr)
		{
			delete val;
			val = nullptr;
		}
	}
};
