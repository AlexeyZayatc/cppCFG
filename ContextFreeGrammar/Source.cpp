#include <iostream>
#include "CFG.h"
using namespace std;

int main() {
	CFG cfgMath = CFG(
		{"E","T","F"},
		{"+","*","(",")","a"},
		{
		  {"T", {"T*F","F"}},
		  {"F", {"(E)","a"}},
		  {"E", {"E+T","T"}}
		},
		"E"
	);

	CFG   testcase1(
		{ "S" },
		{ "a","b" },
			{
				{"S", {"aSbS","bSaS",""}},
			},
			"S"
			);
	CFG   testcase1Answer(
		{ "S", "S\'" },
		{ "a","b" },
			{
				{"S", {"aSbS","bSaS","abS","aSb","ab","ba","bSa","baS"}},
				{"S\'", {"S",""}}
			},
		"S\'"
	);
	testcase1 = testcase1.removeLambdaRules();
	testcase1.printCFG();
	testcase1Answer.printCFG();
	cout << (testcase1 == testcase1Answer);
	CFG aaa;
	aaa.printCFG();
	return 0;

}