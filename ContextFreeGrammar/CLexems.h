#pragma once
#include <map>
#include <string>
using namespace std;
const enum c_typeOfLexem {
	INTNUMBER, FLOATNUMBER,
	IDENTIFIER,
	CHAR, STRING,
	IF, ELSE, SWITCH, CASE, DEFAULT,
	WHILE, FOR, DO,
	TYPE,
	TRUE, FALSE,
	SET, PLUS, MINUS, DIVISION, MULTIPLY, REMAINDER,
	LRBRACKET, RRBRACKET, LSBRACKET, RSBRACKET, LCBRACKET, RCBRACKET,
	COMMA, LESS, GREATER,  DECIMALPOINT, SEMICOLON, COLON, OR, AND, NOT,
	SCANF, PRINTF,
	BREAK, CONTINUE, RETURN,
	ENDOFFILE
};

const string C_PRESENTATION[] = {
	"integer","float",
	"id",
	"char", "string",
	"if","else","switch","case", "default",
	"while","for", "do",
	"type",
	"true","false",
	"set","plus","minus","division","multiply","rem",
	"lrbracket","rrbracket","lsbracket","rsbracket", "lcbracket","rcbracket", 
	"comma","less","greater","point","semicolon","colon", "OR", "AND", "NOT",
	"scanf", "printf",
	"break","continue","return",
	"EOF"
};

static const map<string, c_typeOfLexem> C_KEYWORDS = {
	{"if", IF},
	{"else", ELSE},
	{"do", DO},
	{"while", WHILE},
	{"for", FOR},
	{"break", BREAK},
	{"continue",CONTINUE},
	{"return", RETURN},
	{"float", TYPE},
	{"int", TYPE},
	{"char", TYPE},
	{"bool", TYPE},
	{"switch", SWITCH},
	{"case", CASE},
	{"default", DEFAULT}
};

static const map<string, c_typeOfLexem> C_RESERVEDNAMES = {
	{"true", TRUE},
	{"false", FALSE},
	{"scanf", SCANF},
	{"printf", PRINTF}
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
	{':', COLON},
	{']',RSBRACKET},
	{'{',LCBRACKET},
	{'}',RCBRACKET},
	{';',SEMICOLON},
	{',',COMMA},
	{'<', LESS},
	{'>', GREATER},
	{'.', DECIMALPOINT},
	{'|', OR},
	{'&', AND},
	{'^', NOT}
};