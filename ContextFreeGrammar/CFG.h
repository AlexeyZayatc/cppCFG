#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <set>
#include <map>
#include <algorithm>


using namespace std;
struct Token {
	Token() {
		mLexem = "";
		mLexemType = "";
	}
	Token(const string& lxm, const string& lxmType)
		: mLexem(lxm), mLexemType(lxmType) {}
	Token(const Token& tkn) noexcept
		: mLexem(tkn.mLexem), mLexemType(tkn.mLexemType) {}
	Token(Token&& tkn) noexcept :
		mLexem(move(tkn.mLexem)),
		mLexemType(move(tkn.mLexemType))
	{}
	Token& operator=(const Token& other) noexcept {
		if (this == &other)
			return *this;
		mLexem = other.mLexem;
		mLexemType = other.mLexemType;
		return *this;
	}
	Token& operator=(Token&& other) noexcept {
		mLexem = move(other.mLexem);
		mLexemType = move(other.mLexemType);
		return *this;
	}
	strong_ordering operator<=> (const Token&) const = default;
	friend ostream& operator <<(ostream& ostr, const Token& token);
	string mLexem;
	string mLexemType;
};

typedef vector<vector<Token>> ruleRHS;
typedef map<Token, ruleRHS> ruleDict;
class CFG
{
public:
	CFG(const set<string>& nonTerminals,
		const set<string>& terminals,
		const map<string, vector<string>>& rules,
		const string& axiom) {
		auto convert = [&](set<string> ssv, set<Token>& dst) {
			for (auto&& elem : ssv)
				dst.insert(move(Token(elem, "char")));
		};
		convert(nonTerminals, mNonTerminals);
		convert(terminals, mTerminals);
		for (auto&& rule : rules) {
			auto currentToken = Token(rule.first, "char");
			auto& vectors = rule.second;
			vector<Token> temp;
			for (auto&& vec : vectors) {
				temp.clear();
				if (vec.empty()) temp.push_back(Token("", "char"));
				for (unsigned i = 0; i < vec.size(); i++) {
					temp.push_back(Token(vec.substr(i, 1), "char"));
				}
				mRules[currentToken].push_back(temp);
			}
		}
		mAxiom = Token(axiom, "char");
	}
	CFG(const set<Token>& nonTerminals,
		const set<Token>& terminals,
		const ruleDict& rules,
		const Token& axiom)
		: mNonTerminals(nonTerminals), mTerminals(terminals), mRules(rules), mAxiom(axiom) {}
	CFG(const CFG& other)
		: mNonTerminals(other.mNonTerminals), mTerminals(other.mTerminals),
		mRules(other.mRules), mAxiom(other.mAxiom) {}
	CFG() {
		(*this) = emptyLanguage();
	}
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
	wstring toWString() const;
private:
	bool isRuleContainOnlyGoodTokens(const vector<Token>& rhs, const set<Token>& goodTokens) noexcept;
	void recursivePushBack(ruleRHS& result, vector<Token> tempChain, vector<Token> chain, unsigned adjPoint);
	set<Token> getLambdaNonTerminals();
	set<Token> getGoodNonTerminals();
	set<Token> getTerminalNTForLambda(set<Token> lambdaNT);
	set<Token> mNonTerminals;
	set<Token> mTerminals;
	ruleDict mRules;
	Token mAxiom;
};