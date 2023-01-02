#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <set>
#include <map>
#include <algorithm>
using namespace std;

struct Token {
	Token();
	Token(const string& lxm, const string& lxmType) noexcept;
	Token(const Token& tkn) noexcept;
	Token(Token&& tkn) noexcept;
	Token& operator=(const Token& other) noexcept;
	Token& operator=(Token&& other) noexcept;
	strong_ordering operator<=> (const Token&) const = default;
	friend ostream& operator <<(ostream& ostr, const Token& token);
	string mLexem;
	string mLexemType;
};

typedef vector<vector<Token>> ruleRHS;
typedef map<Token, ruleRHS> ruleDict;
//change logic in chomsky alg
class CFG
{
public:
	CFG(const set<string>& nonTerminals,
		const set<string>& terminals,
		const map<string, vector<string>>& rules,
		const string& axiom);
	CFG(const set<Token>& nonTerminals,
		const set<Token>& terminals,
		const ruleDict& rules,
		const Token& axiom);
	CFG(const CFG& other);
	CFG(const set<string>& nonTerminals,
		const set<string>& terminals,
		const map<string, vector<vector<string>>>& rules,
		const Token& Axiom);
	CFG();
	CFG& operator=(const CFG& other) noexcept;
	CFG& operator=(CFG&& other) noexcept;
	void printCFG();
	bool operator==(const CFG& other) const noexcept;
	bool isLanguageEmpty();
	CFG emptyLanguage();
	CFG removeUnreachableSymbols();
	CFG removeUselessSymbols();
	CFG removeBadNonTerminalsAndRules();
	CFG removeLambdaRules();
	CFG removeLeftRecursion();
	CFG makeChomskyNormalForm();
	wstring toWString() const;
private:
	bool isRuleContainOnlyGoodTokens(const vector<Token>& rhs, const set<Token>& goodTokens) noexcept;
	void recursivePushBack(ruleRHS& result, vector<Token> tempChain, vector<Token> chain, unsigned adjPoint);
	void removeDublicateRules(ruleDict& rules);
	set<Token> getLambdaNonTerminals();
	set<Token> getGoodNonTerminals();
	set<Token> getTerminalNTForLambda(set<Token> lambdaNT);
	set<Token> mNonTerminals;
	set<Token> mTerminals;
	ruleDict mRules;
	Token mAxiom;
};