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
		{ "S","A","B" },
		{ "a","b" },
		{
			{"S",{"A","aaa"}},
			{"A",{"AB","a","BBB"}},
			{"B",{"b"}},
		},
		"S"
		);
	test.printCFG();
	test.removeChainRules().printCFG();
	return 0;

}