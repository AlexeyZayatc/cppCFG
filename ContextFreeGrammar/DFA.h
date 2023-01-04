#pragma once
#include <vector>
#include <set>
using namespace std;
class DFA
{
	enum State;
	set<char> alphabet;
	vector<State> rules;
	const State startState;
	set<State> finalStates;
};

