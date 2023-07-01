﻿#include <iostream>
#include "Parser.h"
#include "CFG.h"
#include "Token.h"
#include "Lexer.h"
#include "Generator.h"

using namespace std;
int main() {
	Lexer forC;
	ifstream simple("simpleProgram.txt", ios::in);
	vector<Token> programTokens = forC.getTokensFromFile(simple);
	Parser parser(programTokens);
	auto program = (parser.parse());
	/*ofstream output("simpleProgramOutput.txt", ios::out);*/
	//output <<  " | LEXEM           | TYPE            | ROW             | COLUMN          | \n";
	//for (auto& token : programTokens)
	//	output << "expectedOutput.push_back(Token(\"" << token.mLexem << "\",\""<< token.mLexemType << "\"," << token.row << "," << token.column << "));" << '\n';
	cout << program->toStr();
	simple.close();
	//output.close();
	ofstream output("simplePascalOutput.txt", ios::out);
	Generator gen(program);
	gen.generate(output);
	output.close();
	return 0;

}