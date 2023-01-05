#pragma once
#include <map>
#include <vector>
#include <set>
#include <fstream>
#include <streambuf>

#include "CFG.h"
#include "CLexerms.h"
#include "Exception.h"
using namespace std;
enum class State {
	NONE,
	QUOTE1,
	QUOTE2,
	SYMBOLS,
	NUMBER,
	IDENTIFIER,
	ENDOFFILE,
	ERROR
};

typedef map<State, map<char, State>> delta;


class DFA
{
public:
	DFA(const set<char>& alphabet, const delta& rules, const State& start, const set<State>& endStates);
	vector<Token> getTokensFromFile(ifstream& fileStream) const ;
private:
	set<char> mAlphabet;
	delta mRules;
	State mStart;
	set<State> mEnd;
};

//making rules) idk how to do it better btw
void makeRules(delta& rls);
set<char> getAlphabet();