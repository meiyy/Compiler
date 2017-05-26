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
	CIntDec,
	CIntOct,
	CIntHex,
	CFloat,
	CString,
	CCharacter,
	Precompile,
	Undefined
};
const char* TypeName[62]= {
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
	"CIntDec",
	"CIntOct",
	"CIntHex",
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
	Word() {}

	Word(string raw,WordType type):raw(raw),type(type)
	{
	}

	bool isEOF()
	{
		return raw == "EOF" && type == Undefined;
	}

	~Word()
	{
	}
};
