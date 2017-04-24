#pragma once
#include<string>
using std::string;

enum WordType
{
	Keyword,
	Identification,
	Operator,
	CintDec,
	CintOct,
	CintHex,
	Cfloat,
	Cstring,
	Ccharacter,
	Undefined,
	Precompile
};
const char* TypeName[11]= {
	"Keyword",
	"Identifier",
	"Operator",
	"CintDec",
	"CintOct",
	"CintHex",
	"Cfloat",
	"Cstring",
	"Ccharacter",
	"Undefined",
	"Precompile"
};

class Word
{
public:
	string raw;
	WordType type;
	Word(string raw,WordType type):raw(raw),type(type)
	{
	}

	~Word()
	{
	}
};

