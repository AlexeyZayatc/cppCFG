#include "Lexer.h"

Lexer::Lexer()
	: mTransitionMap(makeTransitions()),
	  mStart(State::NONE) {}

vector<Token> Lexer::getTokensFromFile(ifstream& fileStream) const
{
	vector<Token> tokens;
	State currentState = mStart;
	State previousState;
	char currentChar;
	int curRow = 1;
	int curColumn = 0;
	string lexem;
	char textOpener=0;

	auto getNextChar = [&]() {
		if (currentChar == '\n') {
			curRow += 1;
			curColumn = 0;
		}
		curColumn += 1;
		return fileStream.get();
	};

	auto getNextState = [&]() {
		return mTransitionMap.at(previousState).at(currentChar);
	};

	auto getTypeOfLexem = [&](const string& lxm) {
		if (lxm.size() == 1 && (C_SYMBOLS.contains(lxm[0])))
			return (C_SYMBOLS.at(lxm[0]));
		else if (C_RESERVEDNAMES.contains(lxm))
			return (C_RESERVEDNAMES.at(lxm));
		else if (C_KEYWORDS.contains(lxm))
			return (C_KEYWORDS.at(lxm));
		else {
			if (previousState == State::TEXT) {
				if (currentChar=='\'') return (CHAR);
				return STRING;
			}
			if ((lxm[0] >= 'a' && lxm[0] <= 'z') || (lxm[0] >= 'A' && lxm[0] <= 'Z') || (lxm[0]=='_'))
				return (IDENTIFIER);
			if (lxm.find('.') != string::npos)
				return (FLOATNUMBER);
		}
		return (INTNUMBER);
	};

	while (currentState!=State::ENDOFFILE) {
		previousState = currentState;
		currentChar = getNextChar();
		if (currentChar > 0) {
			currentState = getNextState();
			if (currentState == State::TEXT && previousState != State::TEXT)
				textOpener = currentChar;
		}
		else {
			if(previousState==State::TEXT) throw Exception("Error while lexing:\n row: " + to_string(curRow)
				+ "\n column:" + to_string(curColumn)
				+ "\n char: " + currentChar);
			currentState = State::ENDOFFILE;
		}
		if ((currentState != previousState || currentState == State::SYMBOLS) 
			&& !lexem.empty())
			{
			if (previousState == State::TEXT) {
				if (lexem.back() == '\\' || currentChar != textOpener) 
				{
					lexem.push_back(currentChar);
					currentState = State::TEXT;
					continue;
				}
				else lexem.erase(0,1);
				currentState = State::NONE;
			}
			string typeOfLexem = C_PRESENTATION[getTypeOfLexem(lexem)];
			tokens.push_back(Token(lexem,typeOfLexem,curRow,curColumn));
			lexem.clear();
		}
		if(currentState==State::NONE)
			lexem.clear();
		else
			lexem.push_back(currentChar);		
		if (currentState == State::ERROR) {
			throw Exception("Error while lexing:\n row: " + to_string(curRow)
				+ "\n column:" + to_string(curColumn)
				+ "\n char: " + currentChar);
		}
	}
	Token endOfFile("EOF", C_PRESENTATION[ENDOFFILE], curRow, curColumn);
	tokens.push_back(endOfFile);
	return tokens;
}

transitionMap Lexer::makeTransitions()
{
	transitionMap transitions;
	map<char, State> forNone;
	map<char, State> forIdentifier;
	map<char, State> forNumber;
	map<char, State> forText;
	map<char, State> forSymbols;

	//initialization of forStates
	for (int i = 0; i < 128; i++)
	{
		forNone[char(i)] = State::ERROR;
		forIdentifier[char(i)] = State::ERROR;
		forNumber[char(i)] = State::ERROR;
		forText[char(i)] = State::TEXT;
	}

	//transitions for NONE state
	{
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
		forNone['\''] = State::TEXT;
		forNone['\"'] = State::TEXT;
		transitions[State::NONE] = forNone;
	}

	//transitions for Identifier state
	{
		for (char ch = 'a'; ch <= 'z'; ch++)
			forIdentifier[ch] = State::IDENTIFIER;
		for (char ch = 'A'; ch <= 'Z'; ch++)
			forIdentifier[ch] = State::IDENTIFIER;
		for (char ch = '0'; ch <= '9'; ch++)
			forIdentifier[ch] = State::IDENTIFIER;
		for (auto& symbPair : C_SYMBOLS)
			forIdentifier[symbPair.first] = State::SYMBOLS;
		forIdentifier['_'] = State::IDENTIFIER;
		forIdentifier[' '] = State::NONE;
		forIdentifier['\n'] = State::NONE;
		forIdentifier['\t'] = State::NONE;
		transitions[State::IDENTIFIER] = forIdentifier;
	}
	//transitions for Number state
	{
		for (char ch = '0'; ch <= '9'; ch++)
			forNumber[ch] = State::NUMBER;
		for (auto& symbPair : C_SYMBOLS)
			forNumber[symbPair.first] = State::SYMBOLS;
		forNumber['.'] = State::NUMBER;
		forNumber[' '] = State::NONE;
		forNumber['\n'] = State::NONE;
		forNumber['\t'] = State::NONE;
		transitions[State::NUMBER] = forNumber;
	}
	//transitions for Symbol state
	{
		forSymbols = forNone;
		transitions[State::SYMBOLS] = forSymbols;
	}

	//transitions for Text state
	{
		forText['\''] = State::NONE;
		forText['\"'] = State::NONE;
		transitions[State::TEXT] = forText;
	}

	return transitions;
}

