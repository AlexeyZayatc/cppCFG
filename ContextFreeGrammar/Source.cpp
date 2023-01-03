#include <iostream>
#include "CFG.h"
using namespace std;

int main() {
	CFG chomskyTest(
		{"S","A","B"},
		{"a","b"},
		{
			{"S",{"aAB","BA"}},
			{"A",{"BBB","a"}},
			{"B",{"AS","b"}}
		},
		"S"
	);/*
	chomskyTest.printCFG();
    chomskyTest.makeChomskyNormalForm().printCFG();*/
	CFG chomskyTest2(
		{"S","A","B"},
		{"a","b"},
		{
			{"S",{"aA","baaA","AaaA"}},
			{"A",{"AB","a","BBB"}},
			{"B",{"bb","BA"}},
		},
		"S"
	);
	chomskyTest2.printCFG();
	chomskyTest2.makeChomskyNormalForm().printCFG();
	CFG test(
		{ "D","E","F" },
		{ "+","*","(",")","a" },
				{
				  {"E", {"E*F","F"}},
				  {"F", {"(D)","a"}},
				  {"D", {"D+E","E"}}
				},
		"D"
	);
	test.printCFG();
	test = test.removeChainRules();
	test.printCFG();
	test = test.removeUselessSymbols();
	test.printCFG();
	test = test.removeLeftRecursion();
	test.printCFG();
	test = test.makeGreibachNormalForm();
	test.printCFG();
	return 0;

}