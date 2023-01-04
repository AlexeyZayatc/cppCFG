#pragma once
#include <map>
#include <vector>
#include <set>
#include <fstream>
#include <streambuf>

#include "CFG.h"
#include "CLexerms.h"

using namespace std;


static enum State {
	NONE,
	QUOTE1,
	QUOTE2,
	SYMBOLS,
	NUMBER,
	IDENTIFIER,
	KEYWORD = 5,
	RESERVEDNAMES = 5
};

typedef map<State, map<char, State>> delta;

class DFA
{
public:
	DFA(const set<char>& alphabet, const delta& rules, const State& start, const set<State>& endStates);
	vector<Token> getTokensFromFile(ifstream& fileStream);
private:
	set<char> mAlphabet;
	delta mRules;
	State mStart;
	set<State> mEnd;
};

