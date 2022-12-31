#include "pch.h"
#include "CppUnitTest.h"
#include "../ContextFreeGrammar/CFG.cpp"

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
	TEST_CLASS(UnitTest)
	{
	public:
		TEST_METHOD(isLanguageEmpty)
		{
			CFG notEmptyCFG({ "A", "B" },
				{ "0", "1" },
				{
					{"A", {"B"}},
					{"B", {"1"}}
				},
				"A"
			);
			CFG  emptyCFGTerminalChainAchieved(
				{ "A", "B", "S", "C", "D" },
				{ "a", "b", "c", "d" },
				  {
					  {"S", {"aAa"}} ,
				  {"A", {"bBb"}} ,
				  {"B", {"cCc"}} ,
				  {"C", {"dDd"}} ,
				  {"D", {"abcd"}}
				  },
				"S"
			);
			CFG notEmptyCFGTerminalChainWithExtraRoutes(
				{ "A", "B", "S", "C", "D", "F" },
				{ "a", "b", "c", "d" },
				{ {"S", {"aAa"}} , {"A", {"bBb", "Cdd"}} , {"B", {"cDc"}} , {"C", {"F"}} , {"D", {"abcd"}} },
				"S"
			);
			CFG emptyCFG(
				{ "A" },
				{ "0" },
				{},
				"A"
			);
			CFG   emptyCFGNoAxiomRule(
				{ "A", "B", "S" },
				{ "a", "b", "c" },
				{ {"A", {"a"}}, {"B", {"b", "c", "A"}} },
				"S"
			);
			CFG       emptyCFGNoTerminalChain(
				{ "A", "B", "S", "C", "D" },
				{ "a", "b", "c", "d" },
				{ {"S", {"aAa"} }, {"A", { "bBb" }
					   }, {"B", { "cCc" }
					  }, {"C", { "dDd" }
						} },
				"S"
			);
			CFG   emptyCFGNoTerminalChainRecursion(
				{ "A", "B", "S", "C", "D" },
				{ "a", "b", "c", "d" },
				{ {"S",{"aAa"} }, {"A", { "bBb" }}, {"B", { "cCc" }}, {"C", { "dAd" }} },
				"S"
			);
			CFG   emptyCFGNoRuleToGoodSymbol(
				{ "A", "B", "C", "S" },
				{ "a", "b", "c" },
				{ { "S", {"AB"} },{ "C", { "aaa" }},{ "A", { "BBB" }} },
				"S"
			);
			CFG  emptyCFGLambdaRule({ "A" }, { "0" }, { {"A", {""}} }, "A");
			Assert::IsTrue(emptyCFG.isLanguageEmpty());
			Assert::IsTrue(emptyCFGNoAxiomRule.isLanguageEmpty());
			Assert::IsTrue(emptyCFGNoTerminalChain.isLanguageEmpty());
			Assert::IsTrue(emptyCFGNoTerminalChainRecursion.isLanguageEmpty());
			Assert::IsTrue(emptyCFGNoRuleToGoodSymbol.isLanguageEmpty());
			Assert::IsTrue(emptyCFGLambdaRule.isLanguageEmpty());
			Assert::IsFalse(notEmptyCFG.isLanguageEmpty());
			Assert::IsFalse(notEmptyCFGTerminalChainWithExtraRoutes.isLanguageEmpty());
			Assert::IsFalse(emptyCFGTerminalChainAchieved.isLanguageEmpty());
		}
		TEST_METHOD(removeUnreachableSymbols) {

			CFG testcase1(
				{ "S" },
				{ "1", "0" },
			{
				{"S", {"0", "1", "0S", "1S"}}
			},
				"S"
			);

			CFG attemp = testcase1.removeUnreachableSymbols();
			Assert::AreEqual(attemp, testcase1);

			CFG   testcase2(
				{ "S", "A", "B", "C" },
				{ "1", "2", "3" },
			{
				{"S", {"A"}},
				{"A" , {"B"}},
				{"B" , {"C"}}
			},
				"S"
			);
			CFG testcase2answer(
				{ "S", "A", "B", "C" },
				set<string>(),
				{
					{"S", {"A"}},
					{"A", {"B"}},
					{"B", {"C"}}
				},
				"S"
			);
			attemp = testcase2.removeUnreachableSymbols();
			Assert::AreEqual(attemp, testcase2answer);

			CFG testcase3(
				{ "S", "A", "B", "C" },
				{ "a", "b", "c" },
			{
				{"S", {"ab", "a"}} ,
				{"A" , {"B"} },
				{"B" , {"C"}}
			},
				"S"
			);
			CFG testcase3answer(
				{ "S" },
				{ "a", "b" },
			{
				{"S", {"ab", "a"}}
			},
				"S"
			);
			attemp = testcase3.removeUnreachableSymbols();
			Assert::AreEqual(testcase3answer, attemp);

			CFG testcase4(
				{ "S", "A", "B", "C" },
				{ "a", "b", "c" },
			{
				{"S", {"S"}   },
				{"A" , {"bBa"}} ,
				{"B" , {"cCa"}} ,
				{"C" , {"ccc"}}
			},
				"S"
			);
			CFG testcase4answer(
				{ "S" },
				set<string>(),
				{
					{"S", {"S"}}
				},
				"S"
			);
			attemp = testcase4.removeUnreachableSymbols();
			Assert::AreEqual(testcase4answer, attemp);

			CFG testcase5(
				{ "E", "F", "T" },
				{ "+", "*", "(", ")", "a" },
			{
				{"E", {"E+T", "T"} },
				{"F" , {"(E)", "a"}}
			},
				"E"
			);
			CFG testcase5answer(
				{ "E", "T" },
				{ "+" },
			{
				{"E", {"E+T", "T"}} ,
			},
			"E"
			);
			attemp = testcase5.removeUnreachableSymbols();
			Assert::AreEqual(testcase5answer, attemp);
		}
		TEST_METHOD(removeBadNonTerminalsAndRules) {
			CFG testCase1(
				{ "S", "A", "B", "C", "D", "E", "F", "G" },
				{ "a", "b", "c", "d", "e", "f", "g" },
			{
				{"S", {"aAa", "bbBbb", "abcd"}} ,
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
				{"S", {"aAa", "abcd"}} ,
				{"A", {"cCc"}} ,
				{"C", {"ccc"}} ,
				{"F", {"fffAAA"}}
			},
				"S"
			);
			CFG attemp = testCase1.removeBadNonTerminalsAndRules();
			Assert::AreEqual(attemp, testCase1Answer);
		}
		TEST_METHOD(removeUselessSymbols) {
			CFG noUselessSymbols(
				{ "S" },
				{ "1", "0" },
			{
				{"S", {"0", "1", "0S", "1S"}}
			},
				"S"
			);

			CFG noUselessSymbolsAnswer = noUselessSymbols.removeUselessSymbols();

			Assert::AreEqual(noUselessSymbolsAnswer, noUselessSymbolsAnswer);

			CFG testCase2(
				{ "S", "A", "B" },
				{ "a", "b" },
				{
					{"S", {"a", "A"}},
				{"A", {"AB"}}
				, {"B", {"b"} }
				},
				"S"
			);

			CFG attemp = testCase2.removeUselessSymbols();
			CFG test_case2_answer(
				{ "S" },
				{ "a" },
				{ {"S", {"a"} } },
				"S"
			);
			Assert::AreEqual(attemp, test_case2_answer);

		}
	};
}
