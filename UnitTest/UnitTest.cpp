#include "pch.h"
#include "CppUnitTest.h"
#include "../ContextFreeGrammar/CFG.cpp"
#include "../ContextFreeGrammar/Exception.cpp"
#include "../ContextFreeGrammar/Token.cpp"
#include "../ContextFreeGrammar/DFA.cpp"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {

			template<>
			static std::wstring ToString<CFG>(const CFG& cfg) {
				return cfg.toWString();
			}

			template<>
			static std::wstring ToString<Token>(const Token& tkn) {
				return tkn.toWString();
			}

			template<>
			static std::wstring ToString<vector<Token>>(const vector<Token>& vt) {
				wstring result = L"";
				for (const auto& tk : vt)
					result += tk.toWString() + L"\n";
				return result;
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
	//LEXER
	set<char> alphabet = getAlphabet();
	delta rules = makeRules();
	set<State> endStates = { State::ENDOFFILE, State::ERROR };
	DFA forC(alphabet, rules, State::NONE, endStates);
	ifstream inputFile;

	TEST_CLASS(testLexer) {
		TEST_METHOD(Identifiers) {
			//Output initialization
			vector<Token> expectedOutput;
			{
				expectedOutput.push_back(Token("abc", "id", 1, 4));
				expectedOutput.push_back(Token("aBc", "id", 1, 8));
				expectedOutput.push_back(Token("abC", "id", 1, 12));
				expectedOutput.push_back(Token("aBC", "id", 1, 16));
				expectedOutput.push_back(Token("Bac", "id", 1, 20));
				expectedOutput.push_back(Token("BaC", "id", 1, 24));
				expectedOutput.push_back(Token("BAc", "id", 1, 28));
				expectedOutput.push_back(Token("BAC", "id", 1, 32));
				expectedOutput.push_back(Token("a0a", "id", 1, 36));
				expectedOutput.push_back(Token("A00", "id", 1, 40));
				expectedOutput.push_back(Token("a00", "id", 1, 44));
				expectedOutput.push_back(Token("A0A", "id", 1, 48));
				expectedOutput.push_back(Token("_A_", "id", 1, 52));
				expectedOutput.push_back(Token("_Ab", "id", 1, 56));
				expectedOutput.push_back(Token("_A0", "id", 1, 60));
				expectedOutput.push_back(Token("_a_", "id", 1, 64));
				expectedOutput.push_back(Token("_ab", "id", 1, 68));
				expectedOutput.push_back(Token("_a0", "id", 1, 72));
				expectedOutput.push_back(Token("b_", "id", 1, 75));
				expectedOutput.push_back(Token("sab", "id", 2, 4));
				expectedOutput.push_back(Token("bec", "id", 2, 8));
				expectedOutput.push_back(Token("_fds_sf_", "id", 2, 17));
				expectedOutput.push_back(Token("fsdf_dsf", "id", 2, 26));
				expectedOutput.push_back(Token("+", "plus", 2, 28));
				expectedOutput.push_back(Token("Z", "id", 2, 29));
				expectedOutput.push_back(Token("V", "id", 2, 31));
				expectedOutput.push_back(Token("+", "plus", 2, 32));
				expectedOutput.push_back(Token("+", "plus", 2, 34));
				expectedOutput.push_back(Token("Z", "id", 2, 36));
				expectedOutput.push_back(Token("V", "id", 2, 38));
				expectedOutput.push_back(Token("+", "plus", 2, 40));
				expectedOutput.push_back(Token("POBEDA", "id", 3, 7));
				expectedOutput.push_back(Token("EOF", "EOF", 3, 7));
			}
			inputFile.open("../../UnitTest/LexerTests/IdentifierTests.txt", ios::in);
			vector<Token> programTokens = forC.getTokensFromFile(inputFile);
			inputFile.close();
			Assert::AreEqual(expectedOutput, programTokens);
		}
		TEST_METHOD(Numbers) {
			//Output initialization
			vector<Token> expectedOutput;
			{
				expectedOutput.push_back(Token("5", "integer", 1, 2));
				expectedOutput.push_back(Token("25", "integer", 1, 5));
				expectedOutput.push_back(Token("365", "integer", 1, 10));
				expectedOutput.push_back(Token("43242", "integer", 1, 17));
				expectedOutput.push_back(Token("-", "minus", 1, 20));
				expectedOutput.push_back(Token("25325", "integer", 1, 25));
				expectedOutput.push_back(Token("54535", "integer", 1, 32));
				expectedOutput.push_back(Token("+", "plus", 1, 33));
				expectedOutput.push_back(Token("5325.4", "float", 1, 41));
				expectedOutput.push_back(Token("444.4444", "float", 1, 50));
				expectedOutput.push_back(Token("0.44444", "float", 1, 58));
				expectedOutput.push_back(Token("5.5", "float", 2, 4));
				expectedOutput.push_back(Token("4342", "integer", 2, 9));
				expectedOutput.push_back(Token("-", "minus", 2, 11));
				expectedOutput.push_back(Token("5325.43242", "float", 2, 21));
				expectedOutput.push_back(Token("EOF", "EOF", 2, 21));
			}
			inputFile.open("../../UnitTest/LexerTests/NumberTests.txt", ios::in);
			vector<Token> programTokens = forC.getTokensFromFile(inputFile);
			inputFile.close();
			Assert::AreEqual(expectedOutput, programTokens);
			inputFile.open("../../UnitTest/LexerTests/NumberError.txt", ios::in);
			auto func = [&] {
				return forC.getTokensFromFile(inputFile);
			};
			Assert::ExpectException<Exception>(func);
			inputFile.close();

		}
		TEST_METHOD(KeywordsAndReservedNames) {
			//Output initialization
			vector<Token> expectedOutput;
			{
				expectedOutput.push_back(Token("if", "if", 1, 3));
				expectedOutput.push_back(Token("else", "else", 1, 8));
				expectedOutput.push_back(Token("do", "do", 1, 11));
				expectedOutput.push_back(Token("while", "while", 1, 19));
				expectedOutput.push_back(Token("for", "for", 1, 24));
				expectedOutput.push_back(Token("break", "break", 1, 33));
				expectedOutput.push_back(Token("continue", "continue", 1, 43));
				expectedOutput.push_back(Token("return", "return", 1, 54));
				expectedOutput.push_back(Token("float", "type", 1, 65));
				expectedOutput.push_back(Token("int", "type", 1, 73));
				expectedOutput.push_back(Token("char", "type", 1, 83));
				expectedOutput.push_back(Token("bool", "type", 1, 91));
				expectedOutput.push_back(Token("switch", "switch", 1, 103));
				expectedOutput.push_back(Token("case", "case", 1, 112));
				expectedOutput.push_back(Token("default", "default", 1, 124));
				expectedOutput.push_back(Token("true", "true", 4, 5));
				expectedOutput.push_back(Token("false", "false", 7, 6));
				expectedOutput.push_back(Token("scanf", "scanf", 8, 8));
				expectedOutput.push_back(Token("printf", "printf", 9, 8));
				expectedOutput.push_back(Token("EOF", "EOF", 9, 8));
			}
			inputFile.open("../../UnitTest/LexerTests/KeywordsAndReservedNamesTests.txt", ios::in);
			vector<Token> programTokens = forC.getTokensFromFile(inputFile);
			inputFile.close();
			Assert::AreEqual(expectedOutput, programTokens);
		}
		TEST_METHOD(Symbols) {
			//Output initialization
			vector<Token> expectedOutput;
			{
			expectedOutput.push_back(Token("a", "id", 1, 2));
			expectedOutput.push_back(Token("=", "set", 1, 3));
			expectedOutput.push_back(Token("5", "integer", 1, 4));
			expectedOutput.push_back(Token("a", "id", 2, 2));
			expectedOutput.push_back(Token("+", "plus", 2, 4));
			expectedOutput.push_back(Token("B", "id", 2, 5));
			expectedOutput.push_back(Token("d", "id", 3, 2));
			expectedOutput.push_back(Token("-", "minus", 3, 3));
			expectedOutput.push_back(Token("5", "integer", 3, 5));
			expectedOutput.push_back(Token("+", "plus", 3, 8));
			expectedOutput.push_back(Token("2", "integer", 3, 10));
			expectedOutput.push_back(Token("*", "multiply", 3, 12));
			expectedOutput.push_back(Token("/", "division", 3, 14));
			expectedOutput.push_back(Token("%", "rem", 3, 16));
			expectedOutput.push_back(Token("(", "lrbracket", 3, 18));
			expectedOutput.push_back(Token(")", "rrbracket", 3, 20));
			expectedOutput.push_back(Token("[", "lsbracket", 3, 22));
			expectedOutput.push_back(Token(":", "colon", 3, 24));
			expectedOutput.push_back(Token("]", "rsbracket", 3, 26));
			expectedOutput.push_back(Token("{", "lcbracket", 3, 28));
			expectedOutput.push_back(Token("}", "rcbracket", 3, 30));
			expectedOutput.push_back(Token(";", "semicolon", 3, 32));
			expectedOutput.push_back(Token(",", "comma", 3, 34));
			expectedOutput.push_back(Token("<", "less", 3, 36));
			expectedOutput.push_back(Token(">", "greater", 3, 38));
			expectedOutput.push_back(Token(".", "point", 3, 40));
			expectedOutput.push_back(Token("|", "OR", 3, 42));
			expectedOutput.push_back(Token("&", "AND", 4, 2));
			expectedOutput.push_back(Token("^", "NOT", 4, 4));
			expectedOutput.push_back(Token("EOF", "EOF", 4, 4));
			}
			inputFile.open("../../UnitTest/LexerTests/SymbolsTests.txt", ios::in);
			vector<Token> programTokens = forC.getTokensFromFile(inputFile);
			inputFile.close();
			Assert::AreEqual(expectedOutput, programTokens);
		}

		TEST_METHOD(TEXT) {
			//Output initialization
			vector<Token> expectedOutput;			
			{
				expectedOutput.push_back(Token("sfdsfds", "char", 1, 9));
				expectedOutput.push_back(Token("fdsfsdfds", "string", 1, 21));
				expectedOutput.push_back(Token("a", "char", 1, 25));
				expectedOutput.push_back(Token("fsdfs'", "char", 1, 35));
				expectedOutput.push_back(Token("fdsfsf\\fsfsf", "string", 1, 52));
				expectedOutput.push_back(Token("fsdfds\"fsdfds'f","string",2,20));
				expectedOutput.push_back(Token("a", "string", 2, 24));
				expectedOutput.push_back(Token("ff\"fsdfds","char",2,37));
				expectedOutput.push_back(Token("fsdfdsf'fsdfdsfds", "string", 2, 57));
				expectedOutput.push_back(Token("EOF", "EOF", 2, 58));

			}
			inputFile.open("../../UnitTest/LexerTests/TextTests.txt", ios::in);
			vector<Token> programTokens = forC.getTokensFromFile(inputFile);
			inputFile.close();
			Assert::AreEqual(expectedOutput, programTokens);

			inputFile.open("../../UnitTest/LexerTests/EOFTextTests.txt", ios::in);
			auto func = [&] {
				return forC.getTokensFromFile(inputFile);
			};
			Assert::ExpectException<Exception>(func);
			inputFile.close();
		}
	};
}
