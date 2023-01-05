#pragma once
#include <map>
#include <string>
using namespace std;
//INTNUMBER
//FLOAT NUMBER
//EOF

const enum c_typeOfLexem {
	INTNUMBER, FLOATNUMBER,
	IDENTIFIER,
	CHARA, CHAR,
	IF, ELSE,
	WHILE, FOR,
	TYPE,
	TRUE, FALSE,
	SET, PLUS, MINUS, DIVISION, MULTIPLY, REMAINDER,
	LRBRACKET, RRBRACKET, LSBRACKET, RSBRACKET, LCBRACKET, RCBRACKET,
	COMMA, LESS, GREATER, QUOTE1, QUOTE2, DECIMALPOINT, SEMICOLON,
	RETURN,
	ENDOFFILE
};

const string C_PRESENTATION[] = {
	"integer","float","id","charA","char","if","else","while","for","type","true","false",
	"set","plus","minus","division","multiply","rem","lrbracket","rrbracket","lsbracket","rsbracket",
	"lcbracket","rcbracket", "comma","less","greater","quote1","quote2","point","semicolon","return",
	"EOF"
};

static const map<string, c_typeOfLexem> C_KEYWORDS = {
	{"if", IF},
	{"else", ELSE},
	{"while", WHILE},
	{"for", FOR},
	{"return", RETURN},
	{"float", TYPE},
	{"int", TYPE},
	{"char*", TYPE},
	{"char", TYPE},
};

static const map<string, c_typeOfLexem> C_RESERVEDNAMES = {
	{"true", TRUE},
	{"false", FALSE}
};

static const map<char, c_typeOfLexem> C_SYMBOLS = {
	{'=',SET},
	{'+',PLUS},
	{'-',MINUS},
	{'*',MULTIPLY},
	{'/',DIVISION},
	{'%',REMAINDER},
	{'(',LRBRACKET},
	{')',RRBRACKET},
	{'[',LSBRACKET},
	{']',RSBRACKET},
	{'{',LCBRACKET},
	{'}',RCBRACKET},
	{';',SEMICOLON},
	{',',COMMA},
	{'<', LESS},
	{'>', GREATER},
	{'\'', QUOTE1},
	{'\"', QUOTE2},
	{'.', DECIMALPOINT},
};