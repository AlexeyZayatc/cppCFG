#include "DFA.h"

DFA::DFA(const set<char>& alphabet, const delta& rules, const State& start, const set<State>& endStates)
	: mAlphabet(alphabet), mRules(rules), mStart(start), mEnd(endStates) {}

vector<Token> DFA::getTokensFromFile(ifstream& fileStream)
{
	vector<Token> tokens;
	State currentState = mStart;
	char currentChar;
	while (fileStream.eof()) {
		fileStream.get(currentChar);
		if (currentState == NONE) {
			//checks to switch state
		}
		switch (currentState) {
		case NUMBER:
			//check for digits
			if (currentChar == ';') {
				//stop and add two tokens 
			}
			break;
		}
	}
	return move(tokens);
}
