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

	CFG   testcase2(
		{ "S"},
		{"a","b"},
			{
				{"S", {"aSbS","bSaS",""}},
			},
		"S"
	);

	CFG   testcase3(
		{ "S","A","B","M","N","K"},
		{ "a","b" },
			{
				{"S", {"KNM"}},
				{"A", {""}},
				{"B", {""}},
				{"M", {"AB"}},
				{"N", {"Ab"}},
				{"K", {"ab"}}
			},
			"S"
			);
	CFG   testcase4(
		{ "S","A","B","M","N","K" },
		{ "a","b","c","p"},
			{
				{"S", {"KNM"}},
				{"A", {"", "c"}},
				{"B", {"", "p"}},
				{"M", {"AB"}},
				{"N", {"Ab"}},
				{"K", {"ab"}}
			},
		"S"
	);
	CFG   testcase5(
		{ "S","A","B","M","N","K" },
		{ "a","b" },
			{
				{"S", {"KNM"}},
				{"A", {""}},
				{"B", {""}},
				{"M", {"AB","a"}},
				{"N", {"Ab"}},
				{"K", {"ab"}}
			},
		"S"
	);
	CFG   testcase6(
		{ "S","A","B","M" },
		{ "a","b" ,"c", "k"},
			{
				{"S", {"Mk"}},
				{"A", {"","c"}},
				{"B", {""}},
				{"M", {"AB"}}
			},
		"S"
	);
	CFG attemp = cfgMath;

	//attemp = testcase2.removeLambdaRules();
	//attemp.printCFG();
	//attemp = testcase3.removeLambdaRules();
	//attemp.printCFG();
    //attemp = testcase4.removeLambdaRules();
	//attemp.printCFG();
	//attemp = testcase5.removeLambdaRules();
	//attemp.printCFG();
	attemp = testcase6.removeLambdaRules();
	attemp.printCFG();

	return 0;

}