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
	DFA forC(alphabet, rules, State::NONE,endStates);
	ifstream simple("simpleProgram.txt", ios::in);
	vector<Token> programTokens = forC.getTokensFromFile(simple);
	ofstream output("simpleProgramOutput.txt", ios::out);
	output <<  " | LEXEM           | TYPE            | ROW             | COLUMN          | \n";
	for (auto& token : programTokens)
		output << token << '\n';
	simple.close();
	output.close();
	return 0;

}