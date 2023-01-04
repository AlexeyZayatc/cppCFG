#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <set>
#include <map>
#include <algorithm>

#include "Token.h"
using namespace std;



typedef vector<vector<Token>> ruleRHS;
typedef map<Token, ruleRHS> ruleDict;
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
	void printCFG() const noexcept;
	bool operator==(const CFG& other) const noexcept;
	bool isLanguageEmpty() const;
	CFG emptyLanguage() const;
	CFG removeUnreachableSymbols() const;
	CFG removeUselessSymbols() const;
	CFG removeBadNonTerminalsAndRules() const;
	CFG removeLambdaRules() const;
	CFG removeLeftRecursion() const;
	CFG removeChainRules() const;
	CFG makeChomskyNormalForm() const;
	CFG makeGreibachNormalForm() const;
	wstring toWString() const;
private:
	bool isRuleContainOnlyGoodTokens(const vector<Token>& rhs, const set<Token>& goodTokens)const ;
	void recursivePushBack(ruleRHS& result,
		vector<Token> tempChain,
		const vector<Token>& chain,
		const unsigned& adjPoint,
		const set<Token>& lambdaNT,
		const set<Token>& terminalNT) const ;
	void removeDublicateRules(ruleDict& rules) ;
	set<Token> getLambdaNonTerminals() const ;
	set<Token> getGoodNonTerminals() const ;
	set<Token> getTerminalNTForLambda(const set<Token>& lambdaNT) const ;
	set<Token> mNonTerminals;
	set<Token> mTerminals;
	ruleDict mRules;
	Token mAxiom;
};