#include "pch.h"
#include "CppUnitTest.h"
#include "../ContextFreeGrammar/CFG.cpp"
#include "../ContextFreeGrammar/Exception.cpp"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {

			template<>
			static std::wstring ToString<CFG>(const CFG& cfg) {
				return cfg.toWString();
			}

		}
	}
}

namespace UnitTest
{
	TEST_CLASS(testIsLanguageEmpty)
	{
	public:
		TEST_METHOD(notEmptySimple)
		{
			CFG notEmptyCFG({ "A", "B" },
				{ "0", "1" },
				{
					{"A", {"B"}},
					{"B", {"1"}}
				},
				"A"
			);
			Assert::IsFalse(notEmptyCFG.isLanguageEmpty());
		}
		TEST_METHOD(notEmptyTerminalChainWithoutExtraRoutes) {
			CFG  notEmptyCFGTerminalChainAchieved(
				{ "A", "B", "S", "C", "D" },
				{ "a", "b", "c", "d" },
				{
					{"S", {"aAa"}},
					{"A", {"bBb"}},
					{"B", {"cCc"}},
					{"C", {"dDd"}},
					{"D", {"abcd"}}
				},
				"S"
			);
			Assert::IsFalse(notEmptyCFGTerminalChainAchieved.isLanguageEmpty());
		}
		TEST_METHOD(notEmptyWithExtraRoutes)
		{
			CFG notEmptyCFGTerminalChainWithExtraRoutes(
				{ "A", "B", "S", "C", "D", "F" },
				{ "a", "b", "c", "d" },
				{
					{"S", {"aAa"}},
					{"A", {"bBb", "Cdd"}},
					{"B", {"cDc"}},
					{"C", {"F"}},
					{"D", {"abcd"}}
				},
				"S"
			);
			Assert::IsFalse(notEmptyCFGTerminalChainWithExtraRoutes.isLanguageEmpty());
		}
		TEST_METHOD(emptyLanguage)
		{
			CFG emptyCFG(
				{ "A" },
				{ "0" },
				{},
				"A"
			);
			Assert::IsTrue(emptyCFG.isLanguageEmpty());
		}
		TEST_METHOD(emptyLanguageNoAxiomRule) {
			CFG   emptyCFGNoAxiomRule(
				{ "A", "B", "S" },
				{ "a", "b", "c" },
				{
					{"A", {"a"}},
					{"B", {"b", "c", "A"}}
				},
				"S"
			);
			Assert::IsTrue(emptyCFGNoAxiomRule.isLanguageEmpty());
		}
		TEST_METHOD(emptyLanguageNoTerminalChain) {
			CFG       emptyCFGNoTerminalChain(
				{ "A", "B", "S", "C", "D" },
				{ "a", "b", "c", "d" },
				{
					{"S", {"aAa"}},
					{"A", { "bBb" }},
					{"B", { "cCc" }},
					{"C", { "dDd" }}
				},
				"S"
			);
			Assert::IsTrue(emptyCFGNoTerminalChain.isLanguageEmpty());
		}
		TEST_METHOD(emptyLanguageWithRecursion)
		{
			CFG   emptyCFGNoTerminalChainRecursion(
				{ "A", "B", "S", "C", "D" },
				{ "a", "b", "c", "d" },
				{
					{"S", {"aAa"}},
					{"A", { "bBb" }},
					{"B", { "cCc" }},
					{"C", { "dAd" }}
				},
				"S"
			);
			Assert::IsTrue(emptyCFGNoTerminalChainRecursion.isLanguageEmpty());
		}
		TEST_METHOD(emptyLanguageNoRuleToGoodToken) {
			CFG   emptyCFGNoRuleToGoodSymbol(
				{ "A", "B", "C", "S" },
				{ "a", "b", "c" },
				{
					{"S", {"AB"}},
					{"C", { "aaa" }},
					{"A", { "BBB" }}
				},
				"S"
			);
			Assert::IsTrue(emptyCFGNoRuleToGoodSymbol.isLanguageEmpty());
		}
		TEST_METHOD(emptyLanguageAxiomLambdaRule)
		{
			CFG  emptyCFGLambdaRule(
				{ "A" },
				{ "0" },
				{
					{"A", {""}}
				},
				"A");
			Assert::IsTrue(emptyCFGLambdaRule.isLanguageEmpty());
		}
	};
	TEST_CLASS(testRemoveUnreachableSymbols) {
public:
	TEST_METHOD(grammarWithoutUnrechableSymbols) {
		CFG testCase1(
			{ "S" },
			{ "1", "0" },
			{
				{"S", {"0", "1", "0S", "1S"}}
			},
			"S"
		);
		Assert::AreEqual(testCase1, testCase1.removeUnreachableSymbols());

	}
	TEST_METHOD(grammarWithoutTerminalChains) {
		CFG   testCase2(
			{ "S", "A", "B", "C" },
			{ "1", "2", "3" },
			{
				{"S", {"A"}},
				{"A" , {"B"}},
				{"B" , {"C"}}
			},
			"S"
		);
		CFG testCase2answer(
			{ "S", "A", "B", "C" },
			set<string>(),
			{
				{"S", {"A"}},
				{"A", {"B"}},
				{"B", {"C"}}
			},
			"S"
		);
		Assert::AreEqual(testCase2answer, testCase2.removeUnreachableSymbols());
	}
	TEST_METHOD(onlyAxiomRuleWithOnlyNonTerminalsAreReacheable) {
		CFG testCase3(
			{ "S", "A", "B", "C" },
			{ "a", "b", "c" },
			{
				{"S", {"ab", "a"}},
				{"A" , {"B"} },
				{"B" , {"C"}}
			},
			"S"
		);
		CFG testCase3answer(
			{ "S" },
			{ "a", "b" },
			{
				{"S", {"ab", "a"}}
			},
			"S"
		);
		Assert::AreEqual(testCase3answer, testCase3.removeUnreachableSymbols());
	}
	TEST_METHOD(grammarWithRuleAxiomIntoAxiom) {
		CFG testCase4(
			{ "S", "A", "B", "C" },
			{ "a", "b", "c" },
			{
				{"S" , {"S"}  },
				{"A" , {"bBa"}},
				{"B" , {"cCa"}},
				{"C" , {"ccc"}}
			},
			"S"
		);
		CFG testCase4answer(
			{ "S" },
			set<string>(),
			{
				{"S", {"S"}}
			},
			"S"
		);
		Assert::AreEqual(testCase4answer, testCase4.removeUnreachableSymbols());
	}
	TEST_METHOD(onlyAxiomRulesAreReacheable) {
		CFG testCase5(
			{ "E", "F", "T" },
			{ "+", "*", "(", ")", "a" },
			{
				{"E", {"E+T", "T"} },
				{"F" , {"(E)", "a"}}
			},
			"E"
		);
		CFG testCase5answer(
			{ "E", "T" },
			{ "+" },
			{
				{"E", {"E+T", "T"}} ,
			},
			"E"
			);
		Assert::AreEqual(testCase5answer, testCase5.removeUnreachableSymbols());
	}
	};
	TEST_CLASS(testRemovingBadNonTerminalsAndRules) {
public:
	TEST_METHOD(allCasesInOneGrammar) {
		CFG testCase1(
			{ "S", "A", "B", "C", "D", "E", "F", "G" },
			{ "a", "b", "c", "d", "e", "f", "g" },
				{
					{"S", {"aAa", "bbBbb", "abcd",""}} ,
					{"A", {"cCc", "dDd"}} ,
					{"B", {"bDb", "Eee"}} ,
					{"C", {"ccc", "G"}} ,
					{"D", {"eeEee"}} ,
					{"E", {"gGg"}} ,
					{"G", {"G"}} ,
					{"F", {"fffAAA"}}
				},
			"S"
		);
		CFG	testCase1Answer(
			{ "S", "A", "C", "F" },
			{ "a", "b", "c", "d", "e", "f", "g" },
				{
					{"S", {"aAa", "abcd",""}} ,
					{"A", {"cCc"}} ,
					{"C", {"ccc"}} ,
					{"F", {"fffAAA"}}
				},
			"S"
		);
		Assert::AreEqual(testCase1Answer, testCase1.removeBadNonTerminalsAndRules());
	}
	};
	TEST_CLASS(testRemoveUselessSymbols) {
public:
	TEST_METHOD(grammarWithoutUselessSymbols) {
		CFG noUselessSymbols(
			{ "S" },
			{ "1", "0" },
				{
					{"S", {"0", "1", "0S", "1S"}}
				},
			"S"
		);
		Assert::AreEqual(noUselessSymbols, noUselessSymbols.removeUselessSymbols());
	}
	TEST_METHOD(grammarWithOnlyOneGoodRule) {
		CFG testCase2(
			{ "S", "A", "B" },
			{ "a", "b" },
				{
					{"S", {"a", "A"}},
					{"A", {"AB"}},
					{"B", {"b"} }
				},
			"S"
		);
		CFG testCase2Answer(
			{ "S" },
			{ "a" },
				{
					{"S", {"a"} }
				},
			"S"
		);
		Assert::AreEqual(testCase2Answer, testCase2.removeUselessSymbols());
	}
	};
	TEST_CLASS(testRemoveLambdaRules) {
		TEST_METHOD(grammarWithAxiomLambdaRule) {

			CFG   testCase1(
				{ "S" },
				{ "a","b" },
				{
					{"S", {"aSbS","bSaS",""}},
				},
				"S"
				);
			CFG   testCase1Answer(
				{ "S", "S\'" },
				{ "a","b" },
				{
					{"S", {"aSbS","bSaS","abS","aSb","ab","ba","bSa","baS"}},
					{"S\'", {"S",""}}
				},
				"S\'"
			);
			Assert::AreEqual(testCase1Answer, testCase1.removeLambdaRules());
		}
		TEST_METHOD(lambdaNonTerminalsAreRemovedFromEverything) {

			CFG   testCase2(
				{ "S","A","B","M","N","K" },
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
			CFG   testCase2Answer(
				{ "S","A","B","M","N","K" },
				{ "a","b" },
				{
					{"S", {"KN"}},
					{"N", {"b"}},
					{"K", {"ab"}}
				},
				"S"
			);
			Assert::AreEqual(testCase2Answer, testCase2.removeLambdaRules());
		}
		TEST_METHOD(lambdaNonTerminalsHaveTerminalChainRule) {
			CFG   testCase3(
				{ "S","A","B","M","N","K" },
				{ "a","b","c","p" },
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
			CFG   testCase3Answer(
				{ "S","A","B","M","N","K" },
				{ "a","b","c","p" },
				{
					{"S", {"KNM","KN"}},
					{"A", {"c"}},
					{"B", {"p"}},
					{"M", {"AB","A","B"}},
					{"N", {"Ab","b"}},
					{"K", {"ab"}}
				},
				"S"
			);
			Assert::AreEqual(testCase3Answer, testCase3.removeLambdaRules());
		}
		TEST_METHOD(grammarHasLambdaNonTerminalsWithTerminalChainRule) {
			CFG   testCase4(
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
			CFG   testCase4Answer(
				{ "S","A","B","M","N","K" },
				{ "a","b" },
				{
					{"S", {"KN","KNM"}},
					{"M", {"a"}},
					{"N", {"b"}},
					{"K", {"ab"}}
				},
				"S"
			);
			Assert::AreEqual(testCase4Answer, testCase4.removeLambdaRules());
		}
		TEST_METHOD(grammarHasLambdaNonTerminalsAndLambdaNonTerminalsWithTerminalChain) {
			CFG   testCase5(
				{ "S","A","B","M" },
				{ "c", "k" },
				{
					{"S", {"Mk"}},
					{"A", {"","c"}},
					{"B", {""}},
					{"M", {"AB"}}
				},
				"S"
			);
			CFG   testCase5Answer(
				{ "S","A","B","M" },
				{ "c", "k" },
				{
					{"S", {"Mk","k"}},
					{"A", {"c"}},
					{"M", {"A"}}
				},
				"S"
			);
			Assert::AreEqual(testCase5Answer, testCase5.removeLambdaRules());
		}
	};
	TEST_CLASS(testRemoveChainRules) {
		TEST_METHOD(languageEmpty) {
			CFG languageEmpty(
				{ "S","A","B" },
				{ "a","b" },
				{
					{ "A",{"a","ab","B"} },
					{ "B",{"A","Abb","bbaa"} }
				},
				"S"
			);

			CFG languageEmptyAnswer(
				{ "S" },
				{ "a","b" },
				{
					{ "S",{} }
				},
				"S"
			);


			Assert::AreEqual(languageEmptyAnswer, languageEmpty.removeChainRules());
		}
		TEST_METHOD(cycleOneStep) {
			CFG cycleOneStep(
				{ "S", "A" },
				{ "a", "b" },
				{
					{"S", {"S","Aab"}},
					{"A", {"ab","ba"}}
				},
				"S"
			);
			CFG cycleOneStepAnswer(
				{ "S", "A" },
				{ "a", "b" },
				{
					{"S", {"Aab"}},
					{"A", {"ab","ba"}}
				},
				"S"
			);
			Assert::AreEqual(cycleOneStepAnswer, cycleOneStep.removeChainRules());
		}
		TEST_METHOD(cycleMoreSteps) {
			CFG cycleMoreSteps(
				{ "S", "A", "B", "C" },
				{ "a", "b" },
				{
					{"S", {"Bab","Aab"}},
					{"A", {"abC","Cba"}},
					{"B", {"C","A","ba"}},
					{"C", {"A","B","bab"}}
				},
				"S"
			);
			CFG cycleMoreStepsAnswer(
				{ "S", "A", "B", "C" },
				{ "a", "b" },
				{
					{"S", {"Bab","Aab"}},
					{"A", {"abC","Cba"}},
					{"B", {"ba", "bab","abC","Cba"}},
					{"C", {"abC","Cba","ba","bab"}}
				},
				"S"
			);
			Assert::AreEqual(cycleMoreStepsAnswer, cycleMoreSteps.removeChainRules());
		}
		TEST_METHOD(uselessNonTerminal) {
			CFG uselessNonTerminal(
				{ "A", "B", "C" },
				{ "a", "b" },
				{
					{"A", {"Ba","B"}},
					{"B", {"C","bab"}}
				},
				"A"
			);
			CFG uselessNonTerminalAnswer(
				{ "A", "B" },
				{ "a", "b" },
				{
					{"A", {"Ba","bab"}},
					{"B", {"bab"}}
				},
				"A"
			);
			Assert::AreEqual(uselessNonTerminalAnswer, uselessNonTerminal.removeChainRules());
		}
	};
	TEST_CLASS(testRemoveLeftRecursion) {
		TEST_METHOD(noLeftRecursion) {
			CFG originalGrammar(
				{ "A", "B" },
				{ "a", "b", "c", "d" },
				{
					{ "A", {"aBb", "cd"} },
					{ "B", { "c", "d" } }
				},
				"A"
			);
			Assert::AreEqual(originalGrammar, originalGrammar.removeLeftRecursion());
		}
		TEST_METHOD(AxiomHasLeftRecursionButemptyLanguage) {
			CFG testCase2(
				{ "S", "A", "B" },
				{ "c", "d" },
				{
					{"A", {"AB", "Acd"}},
					{"B" , {"c", "d"}}
				},
				"S"
			);

			CFG testCase2Answer(
				{ "S" },
				{ },
				{
					{"S", {""} }
				},
				"S"
			);
			Assert::AreEqual(testCase2Answer, testCase2.removeLeftRecursion());
		}
		TEST_METHOD(axiomHasRecursionOtherNonTerminalHasNot) {
			CFG testCase3(
				{ "B","A" },
				{ "c", "d" },
				{
					{"A", {"Bc", "Acd"}} ,
					{"B" , {"c", "d"}}
				},
				"A"
			);
			CFG testCase3Answer(
				{ "B","A","A\'" },
				{ "c","d" },
				{
					{"A",{
						{"B","c"},
						{"B","c","A\'"}
				}},
					{"B",{
						{"c"},
						{"d"}
				}},
					{"A\'",{
						{"c","d"},
						{"c","d","A\'"}
				}},
				},
				Token("A", "char")
			);
			Assert::AreEqual(testCase3Answer, testCase3.removeLeftRecursion());
		}
		TEST_METHOD(axiomHasNoRecursionOtherNonTerminalHasRecursionButEmptyLanguage) {
			CFG testCase4(
				{ "A", "B" },
				{ "c", "d" },
				{
					{"A", {"Bc"}} ,
					{"B" , {"Bc", "Bd"}}
				},
				"A"
			);
			CFG testCase4Answer(
				{ "S" },
				{ },
				{
					{"S", {""}}
				},
				"S"
			);
			Assert::AreEqual(testCase4Answer, testCase4.removeLeftRecursion());
		}
		TEST_METHOD(axiomAndNonTerminalHaveLeftRecursionToEachOther) {

			CFG testCase5(
				{ "B","A" },
				{ "c", "d" },
				{
					{"B" , {"Ac", "Ad"}},
					{"A", {"Bc", "dc"}}
				},
				"A"
			);
			CFG testCase5Answer(
				{ "B","A","B\'" },
				{ "c","d" },
				{
					{"A",{
						{"B","c"},
						{"d","c"}
				}},
					{"B",{
						{"d","c","c"},
						{"d","c","c","B\'"},
						{"d","c","d"},
						{"d","c","d","B\'"}
				}},
					{"B\'",{
						{"c","c"},
						{"c","c","B\'"},
						{"c","d"},
						{"c","d","B\'"}
				}},
				},
				Token("A", "char")
			);
			Assert::AreEqual(testCase5Answer, testCase5.removeLeftRecursion());
		}
		TEST_METHOD(axiomHasLeftRecursionOtherNonTerminalsAreUnreachable) {

			CFG testCase6(
				{ "A", "B" },
				{ "c", "d" },
				{
					{"A", {"Ac", "dc"}} ,
					{"B" , {"Ad"}}
				},
				"A"
			);
			CFG testCase6Answer(
				{ "A","A\'" },
				{ "c","d" },
				{
					{"A",{
						{"d","c"},
						{"d","c","A\'"}
				}},
					{"A\'",{
						{"c"},
						{"c","A\'"},
				}},
				},
				Token("A", "char")
				);
			Assert::AreEqual(testCase6Answer, testCase6.removeLeftRecursion());

		}
	};
	TEST_CLASS(testMakeChomskyForm) {
		TEST_METHOD(noLambdaAxiomRule) {
			CFG testCase1(
				{ "S","A","B" },
				{ "a","b" },
				{
					{"S",{"aAB","BA"}},
					{"A",{"BBB","a"}},
					{"B",{"AS","b"}}
				},
				"S"
			);
			CFG testCase1Answer(
				{ "A","B","S","<AB>","<BB>","a'" },
				{ "a","b" },
				{
			{"S",
				{
				{"B","A"},
				{"a'","<AB>"},
				}},
			{"A",
				{
					{"a"},
					{"B","<BB>"}
				}},
				{"B",{
					{"b"},
					{"A","S"}
				}},
				{"a'",{
					{"a"}
				}},
				{"<AB>",{
					{"A","B"}
				}},
				{"<BB>",{
					{"B","B"}
				}}

				},
				Token("S", "char")
			);
			Assert::AreEqual(testCase1Answer, testCase1.makeChomskyNormalForm());
		}
		TEST_METHOD(axiomHasLambdaRule) {
			CFG testCase2(
				{ "S","A","B" },
				{ "a","b" },
				{
					{"S",{"aA",""}},
					{"A",{"AB","a","BBB"}},
					{"B",{"b"}},
				},
				"S"
				);
			CFG testCase2Answer(
				{ "A","B","S","<BB>","a'","S'" },
				{ "a","b" },
				{
			{"S",
				{
				{"a'","A"}
				}},
			{"A",
				{
					{"a"},
					{"B","<BB>"},
					{"A","B"}
				}},
				{"B",{
					{"b"},
				}},
				{"a'",{
					{"a"}
				}},
				{"<BB>",{
					{"B","B"}
				}}
				},
				Token("S'", "char")
			);

			Assert::AreEqual(testCase2Answer, testCase2.makeChomskyNormalForm());
		}
	};
	TEST_CLASS(testMakeGreibachFrom) {
		TEST_METHOD(arithmeticsExpressions) {
			CFG cfgMath(
				{ "D","E","F" },
				{ "+","*","(",")","a" },
				{
				  {"E", {"E*F","F"}},
				  {"F", {"(D)","a"}},
				  {"D", {"D+E","E"}}
				},
				"D"
			);			
			CFG cfgMathAnswer(
				{ "D","E","F","D\'","E\'",")\'","*\'"},
				{ "+","*","(",")","a" },
				{
				{"D\'",{
					{"+","E"},
					{"+","E","D\'"},
				}},
				{"E\'",{
					{"*","F"},
					{"*","F","E\'"}
				}},
				{"F",{
					{"(","D",")\'"},
					{"a"}
				}},
				{"E",{
					{"(","D",")\'"},
					{"(","D",")\'","E\'"},
					{"a"},
					{"a","E\'"},
				}},
				{"D",{
					{"(","D",")\'"},
					{"(","D",")\'","*\'","F"},
					{"(","D",")\'","D\'"},
					{"(","D",")\'","*\'","F","D\'"},
					{"(","D",")\'","E\'","*\'","F"},
					{"(","D",")\'","E\'","*\'","F","D\'"},
					{"a"},
					{"a","*\'","F"},
					{"a","*\'","F","D\'"},
					{"a","D\'"},
					{"a","E\'","*\'","F"},
					{"a","E\'","*\'","F","D\'"}
				}},
				{")\'",{
						{")"}
				}},
				{"*\'",{
						{"*"}
				}}
				},
				Token("D", "char")
			);
			Assert::AreEqual(cfgMathAnswer, cfgMath.makeGreibachNormalForm());
		}
		TEST_METHOD(randomGrammarWIthoutLeftRecursion) {
			CFG randomGrammar(
				{ "A","B","C" },
				{ "a","b","c" },
				{
					{"A",{"BC","a"} },
					{"B",{"CA","ab"}},
					{"C",{"abcB"}}
				},
				"A"
			);
			CFG randomAnswer(
				{ "A","B","C","b\'","c\'" },
				{ "a","b","c" },
				{
			{"A",
				{
				{"a","b\'","c\'","B","A","C"},
				{"a","b\'","C"},
				{"a"}
				}},
			{"B",
				{
					{"a","b\'","c\'","B","A"},
					{"a","b\'"}
				}},
				{"C",{
					{"a","b\'","c\'","B"},
				}},
				{"b\'",{
					{"b"}
				}},
				{"c\'",{
						{"c"}
				}}
				},
				Token("A", "char")
			);
			Assert::AreEqual(randomAnswer, randomGrammar.makeGreibachNormalForm());
		}
	};
}
