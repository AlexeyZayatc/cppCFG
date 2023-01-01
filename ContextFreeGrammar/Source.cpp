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
	);
	chomskyTest.printCFG();
    chomskyTest.makeChomskyNormalForm().printCFG();
	CFG chomskyTest2(
		{"S","A","B"},
		{"a","b"},
		{
			{"S",{"aA","aaa","aaa","aA","AaaA"}},
			{"A",{"AB","a","BBB","BBB","a"}},
			{"B",{"b","b","BA"}},
		},
		"S"
	);
	chomskyTest2.printCFG();
	CFG test(
		{ "S","A","B" },
		{ "a","b" },
		{
			{"S",{"aA","aaa"}},
			{"A",{"AB","a","BBB"}},
			{"B",{"b"}},
		},
		"S"
		);
	test.printCFG();
	return 0;

}