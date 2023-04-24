#include <iostream>
#include "CFG.h"
#include "Token.h"
#include "DFA.h"
using namespace std;
int main() {
	
	set<char> alphabet = getAlphabet();
	delta rules = makeRules();
	set<State> endStates;
	endStates.insert(State::ENDOFFILE);
	endStates.insert(State::ERROR);
	DFA forC(alphabet, rules, State::NONE,endStates);
	ifstream simple("../UnitTest/LexerTests/EOFTextTests.txt", ios::in);
	vector<Token> programTokens = forC.getTokensFromFile(simple);
	ofstream output("../UnitTest/LexerTests/EOFTextTestsOutput.txt", ios::out);
	//output <<  " | LEXEM           | TYPE            | ROW             | COLUMN          | \n";
	for (auto& token : programTokens)
		output << "expectedOutput.push_back(Token(\"" << token.mLexem << "\",\""<< token.mLexemType << "\"," << token.row << "," << token.column << "));" << '\n';
	simple.close();
	output.close();
	return 0;

}