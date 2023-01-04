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
	//chomskyTest2.makeChomskyNormalForm().printCFG();
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
	CFG   testCase5(
		{ "S","A","B","M" },
		{ "c", "k" },
				{
					{"S", {"Mk", "","kckc","ckck","Bc","BA"}},
					{"A", {"c"}},
					{"B", {""}},
					{"M", {"AB"}}
				},
		"S"
	);
	testCase5.removeLambdaRules().printCFG();
	testCase5.removeLambdaRules().removeChainRules().printCFG();
	testCase5.removeChainRules().printCFG();
	return 0;

}