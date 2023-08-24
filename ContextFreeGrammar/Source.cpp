#include <iostream>
#include "Parser.h"
#include "CFG.h"
#include "Token.h"
#include "Lexer.h"
#include "Generator.h"

using namespace std;
int main() {
	CFG cfgMath(
		{ "T","E","F" },
		{ "+","*","(",")","a" },
				{
				  {"E", {"E*F","F"}},
				  {"T", {"(E)","a"}},
				  {"F", {"F+T","T"}}
				},
		"E"
	);
	auto temp = cfgMath.getCockeYoungerKasamiTable({"(","a","+","a","*","a"});

		return 0;
}