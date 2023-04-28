#pragma once
#include <map>
#include <vector>
#include <set>
#include <fstream>
#include <streambuf>

#include "CFG.h"
#include "CLexems.h"
#include "Exception.h"
using namespace std;
enum class State {
	NONE,
	TEXT,
	SYMBOLS,
	NUMBER,
	IDENTIFIER,
	ENDOFFILE,
	ERROR
};

typedef map<State, map<char, State>> transitionMap;


class Lexer
{
public:
	Lexer();
	vector<Token> getTokensFromFile(ifstream& fileStream) const ;
private:
	transitionMap mTransitionMap;
	State mStart;

	//making rules) idk how to do it better btw
	transitionMap makeTransitions();
};
