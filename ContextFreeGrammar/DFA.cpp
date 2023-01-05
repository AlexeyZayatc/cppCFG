#include "DFA.h"

DFA::DFA(const set<char>& alphabet, const delta& rules, const State& start, const set<State>& endStates)
	: mAlphabet(alphabet), mRules(rules), mStart(start), mEnd(endStates) {}

vector<Token> DFA::getTokensFromFile(ifstream& fileStream) const
{
	vector<Token> tokens;
	State currentState = mStart;
	State previousState;
	char currentChar;
	int curRow = 1;
	int curColumn = 0;
	string lexem;
	auto getNextChar = [&]() {
		if (currentChar == '\n') {
			curRow += 1;
			curColumn = 0;
		}
		fileStream.get(currentChar);
		curColumn += 1;
	};
	auto getTypeOfLexem = [&](const string& lxm) {
		if (lxm.size() == 1 && (C_SYMBOLS.contains(lxm[0])))
			return C_PRESENTATION[int(C_SYMBOLS.at(lxm[0]))];
		else if (C_RESERVEDNAMES.contains(lxm))
			return C_PRESENTATION[int(C_RESERVEDNAMES.at(lxm))];
		else if (C_KEYWORDS.contains(lxm))
			return C_PRESENTATION[int(C_KEYWORDS.at(lxm))];
		else {
			if (currentState == State::QUOTE1)
				return C_PRESENTATION[int(CHAR)];
			if (currentState == State::QUOTE2)
				return C_PRESENTATION[int(CHARA)];
			if ((lxm[0] >= 'a' && lxm[0] <= 'z') || (lxm[0] >= 'A' && lxm[0] <= 'Z'))
				return C_PRESENTATION[int(IDENTIFIER)];
			if (lxm.find('.') != string::npos)
				return C_PRESENTATION[int(FLOATNUMBER)];
		}
		return C_PRESENTATION[int(INTNUMBER)];
	};
	while (fileStream.eof()) {
		previousState = currentState;
		getNextChar();
		currentState = mRules.at(currentState).at(currentChar);
		if (previousState != State::NONE && currentState != previousState) {
			string typeOfLexem = getTypeOfLexem(lexem);
			tokens.push_back(Token(lexem,typeOfLexem,curRow,curColumn));
		}
		if(currentState==State::NONE)
			lexem.clear();
		else
			lexem.push_back(currentChar);		
		if (currentState == State::ERROR) {
			cerr << "ERROR";
			return vector<Token>();
		}
	}
	return tokens;
}

void makeRules(delta& rls)
{
	map<char, State> forNone = {};
	for (int i = 0; i < 255; i++)
		forNone[char(i)] = State::ERROR;
	for (char ch = 'a'; ch <= 'z'; ch++) 
		forNone[ch] = State::IDENTIFIER;
	for (char ch = 'A'; ch <= 'Z'; ch++) 
		forNone[ch] = State::IDENTIFIER;
	forNone['_'] = State::IDENTIFIER;
	for (char ch = '0'; ch <= '9'; ch++)
		forNone[ch] = State::NUMBER;
	forNone['\''] = State::QUOTE1;
	forNone['\"'] = State::QUOTE2;
	forNone[' '] = State::NONE;
	forNone['\n'] = State::NONE;
	forNone['\t'] = State::NONE;
	for (auto& symbPair : C_SYMBOLS)
		forNone[symbPair.first] = State::SYMBOLS;

}
