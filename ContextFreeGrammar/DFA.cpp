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
		currentChar = fileStream.get();
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
			if (previousState == State::QUOTE1)
				return C_PRESENTATION[int(CHAR)];
			if (previousState == State::QUOTE2)
				return C_PRESENTATION[int(CHARA)];
			if ((lxm[0] >= 'a' && lxm[0] <= 'z') || (lxm[0] >= 'A' && lxm[0] <= 'Z'))
				return C_PRESENTATION[int(IDENTIFIER)];
			if (lxm.find('.') != string::npos)
				return C_PRESENTATION[int(FLOATNUMBER)];
		}
		return C_PRESENTATION[int(INTNUMBER)];
	};
	while (!fileStream.eof()) {
		previousState = currentState;
	    getNextChar();
		if (currentChar <= 0)
			break;
		currentState = mRules.at(currentState).at(currentChar);
		if ((currentState != previousState || currentState == State::SYMBOLS) 
			&& !lexem.empty())
			{
			if (previousState == State::QUOTE1)
				lexem.push_back('\'');
			else if (previousState == State::QUOTE2)
				lexem.push_back('\"');
			string typeOfLexem = getTypeOfLexem(lexem);
			tokens.push_back(Token(lexem,typeOfLexem,curRow,curColumn));
			lexem.clear();
		}
		if(currentState==State::NONE)
			lexem.clear();
		else
			lexem.push_back(currentChar);		
		if (currentState == State::ERROR) {
			throw Exception("Error while lexing:\n row: "+to_string(curRow)
				+"\n column:" + to_string(curColumn)
				+ "\n char: " + currentChar);
		}
	}
	Token endOfFile("EOF", C_PRESENTATION[(int)ENDOFFILE], curRow, curColumn);
	tokens.push_back(endOfFile);
	return tokens;
}

void makeRules(delta& rls)
{
	map<char, State> forNone;
	map<char, State> forIdentifier;
	map<char, State> forInteger;
	map<char, State> forQuote1;
	map<char, State> forQuote2;
	map<char, State> forSymbols;
	//for NONE state
	{
		for (int i = 0; i < 128; i++)
			forNone[char(i)] = State::ERROR;
		for (char ch = 'a'; ch <= 'z'; ch++)
			forNone[ch] = State::IDENTIFIER;
		for (char ch = 'A'; ch <= 'Z'; ch++)
			forNone[ch] = State::IDENTIFIER;
		forNone['_'] = State::IDENTIFIER;
		for (char ch = '0'; ch <= '9'; ch++)
			forNone[ch] = State::NUMBER;
		for (auto& symbPair : C_SYMBOLS)
			forNone[symbPair.first] = State::SYMBOLS;
		forNone[' '] = State::NONE;
		forNone['\n'] = State::NONE;
		forNone['\t'] = State::NONE;
		forNone['\''] = State::QUOTE1;
		forNone['\"'] = State::QUOTE2;
		rls[State::NONE] = forNone;
	}
	//for identifier state
	{
		for (int i = 0; i < 128; i++)
			forIdentifier[char(i)] = State::ERROR;
		for (char ch = 'a'; ch <= 'z'; ch++)
			forIdentifier[ch] = State::IDENTIFIER;
		for (char ch = 'A'; ch <= 'Z'; ch++)
			forIdentifier[ch] = State::IDENTIFIER;
		for (char ch = '0'; ch <= '9'; ch++)
			forIdentifier[ch] = State::IDENTIFIER;
		for (auto& symbPair : C_SYMBOLS)
			forIdentifier[symbPair.first] = State::SYMBOLS;
		forIdentifier['*'] = State::IDENTIFIER;
		forIdentifier['_'] = State::IDENTIFIER;
		forIdentifier[' '] = State::NONE;

		rls[State::IDENTIFIER] = forIdentifier;
	}
	//for Integer
	{
		for (int i = 0; i < 128; i++)
			forInteger[char(i)] = State::ERROR;
		for (char ch = '0'; ch <= '9'; ch++)
			forInteger[ch] = State::NUMBER;
		for (auto& symbPair : C_SYMBOLS)
			forInteger[symbPair.first] = State::SYMBOLS;
		forInteger['.'] = State::NUMBER;
		forInteger[' '] = State::NONE;
		rls[State::NUMBER] = forInteger;
	}
	//for Symbol
	{
		forSymbols = forNone;
		rls[State::SYMBOLS] = forSymbols;
	}

	//for '
	for (int i = 0; i < 128; i++)
		forQuote1[char(i)] = State::QUOTE1;
	forQuote1['\''] = State::NONE;
	rls[State::QUOTE1] = forQuote1;
	//for "
	for (int i = 0; i < 128; i++)
		forQuote2[char(i)] = State::QUOTE2;
	forQuote2['\"'] = State::NONE;
	rls[State::QUOTE2] = forQuote2;
}

set<char> getAlphabet() {
	set<char> result;
	for (int i = 0; i < 128; i++)
		result.insert(char(i));
	return result;
}