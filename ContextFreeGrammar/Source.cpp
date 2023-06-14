#include <iostream>
#include "CFG.h"
#include "Token.h"
#include "Lexer.h"
using namespace std;
int main() {
	Lexer forC;
	ifstream simple("simpleProgram.txt", ios::in);
	vector<Token> programTokens = forC.getTokensFromFile(simple);
	ofstream output("simpleProgramOutput.txt", ios::out);
	//output <<  " | LEXEM           | TYPE            | ROW             | COLUMN          | \n";
	for (auto& token : programTokens)
		output << "expectedOutput.push_back(Token(\"" << token.mLexem << "\",\""<< token.mLexemType << "\"," << token.row << "," << token.column << "));" << '\n';
	simple.close();
	output.close();
	return 0;

}