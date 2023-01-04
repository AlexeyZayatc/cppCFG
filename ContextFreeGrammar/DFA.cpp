#include "DFA.h"

DFA::DFA(const set<char>& alphabet, const delta& rules, const State& start, const set<State>& endStates)
	: mAlphabet(alphabet), mRules(rules), mStart(start), mEnd(endStates) {}

vector<Token> DFA::getTokensFromFile(ifstream& fileStream)
{
	vector<Token> tokens;
	State currentState = mStart;
	char* currentChar = new char();
	currentChar = nullptr;
	while (fileStream.eof()) {
		if(currentChar ==nullptr)
			//get new char

		switch (currentState) {
		case State::NONE:
			//check for char and switch state
			break;
		case State::NUMBER:
			//check for digits
			if (*currentChar == ';') {
				//stop and add two tokens
				//also can be + or  ' ' or 
			}
			break;
		}
	}
	delete currentChar;
	return tokens;
}
