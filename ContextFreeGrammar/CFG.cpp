#include <ranges>

#include "CFG.h"
#include "Exception.h"

Token::Token() : mLexem(""), mLexemType("") {}
Token::Token(const string& lxm, const string& lxmType) noexcept
	: mLexem(lxm), mLexemType(lxmType) {}
Token::Token(const Token& tkn) noexcept
	: mLexem(tkn.mLexem), mLexemType(tkn.mLexemType) {}
Token::Token(Token&& tkn) noexcept :
	mLexem(move(tkn.mLexem)),
	mLexemType(move(tkn.mLexemType)) {}
Token& Token::operator=(const Token& other) noexcept {
	if (this == &other)
		return *this;
	mLexem = other.mLexem;
	mLexemType = other.mLexemType;
	return *this;
}
Token& Token::operator=(Token&& other) noexcept {
	mLexem = move(other.mLexem);
	mLexemType = move(other.mLexemType);
	return *this;
}

std::ostream& operator <<(std::ostream& ostr, const Token& token) {
	std::cout << '(' << token.mLexem << ',' << token.mLexemType << ')';
	return ostr;
}

CFG::CFG(const set<string>& nonTerminals,
	const set<string>& terminals,
	const map<string, vector<string>>& rules,
	const string& axiom) {
	//checks for input
	try {
		if (axiom.empty()) {
			throw Exception("Lambda can't be an axiom\n");
		}
		if (!nonTerminals.contains(axiom))
			throw Exception("Non-terminals set doesn't have axiom\n");
		for (auto& nt : nonTerminals)
			if (nt.empty())
				throw Exception("Non-terminals has lambda string");
		for (auto& t : terminals)
			if (t.empty())
				throw Exception("Terminals has lambda string");
		vector<string> intersection;
		set_intersection(nonTerminals.begin(), nonTerminals.end(),
			terminals.begin(), terminals.end(), back_inserter(intersection));
		if (!intersection.empty())
			throw Exception("Terminal and Non-terminal sets intersect\n");
		map<string, vector<string>> rulesCopy = rules;
		std::string cursubStr;
		for (auto& rulePair : rulesCopy) {
			if (!nonTerminals.contains(rulePair.first))
				throw Exception("Rules have lhs non-terminal which is not in non-terminal set\n");
			for (auto& curRule : rulePair.second)
				for (unsigned i = 0; i < curRule.size(); i++)
				{
					cursubStr = curRule.substr(i, 1);
					if (!nonTerminals.contains(cursubStr))
						if (!terminals.contains(cursubStr) && !cursubStr.empty())
							throw Exception("Symbol is not in non-terminals or terminals set\n");
				}
		}
	}
	catch (Exception e) {
		cerr << e.what();
		mAxiom = Token("S", "char");
		mNonTerminals.insert(mAxiom);
		ruleRHS rRHS;
		vector<Token> temp; temp.push_back(Token("", "char"));
		rRHS.push_back(temp);
		mRules[mAxiom] = rRHS;
		return;
	}
	auto convert = [&](const set<string>& setOfStrings, set<Token>& setOfTokens) {
		for (auto&& elem : setOfStrings)
			setOfTokens.insert(move(Token(elem, "char")));
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
	removeDublicateRules(mRules);
	mAxiom = Token(axiom, "char");
}
CFG::CFG(const set<string>& nonTerminals,
	const set<string>& terminals,
	const map<string, vector<vector<string>>>& rules,
	const Token& axiom){
	//checks for input
	try {
		if (axiom.mLexem.empty()) {
			throw Exception("Lambda can't be an axiom\n");
		}
		if (!nonTerminals.contains(axiom.mLexem))
			throw Exception("Non-terminals set doesn't have axiom\n");
		for (auto& nt : nonTerminals)
			if (nt.empty())
				throw Exception("Non-terminals has lambda string");
		for (auto& t : terminals)
			if (t.empty())
				throw Exception("Terminals has lambda string");
		vector<string> intersection;
		set_intersection(nonTerminals.begin(), nonTerminals.end(),
			terminals.begin(), terminals.end(), back_inserter(intersection));
		if (!intersection.empty())
			throw Exception("Terminal and Non-terminal sets intersect\n");
		map<string, vector<vector<string>>> rulesCopy = rules;
		for (auto& rulePair : rulesCopy) {
			if (!nonTerminals.contains(rulePair.first))
				throw Exception("Rules have lhs non-terminal which is not in non-terminal set\n");
			for (auto& curRule : rulePair.second)
				for (unsigned i = 0; i < curRule.size(); i++)
				{
					if (!nonTerminals.contains(curRule[i]))
						if (!terminals.contains(curRule[i]) && !curRule[i].empty())
							throw Exception("Symbol is not in non-terminals or terminals set\n");
				}
		}
	}
	catch (Exception e) {
		cerr << e.what();
		mAxiom = Token("S", "char");
		mNonTerminals.insert(mAxiom);
		ruleRHS rRHS;
		vector<Token> temp; temp.push_back(Token("", "char"));
		rRHS.push_back(temp);
		mRules[mAxiom] = rRHS;
		return;
	}
	auto convert = [&](const set<string>& setOfStrings, set<Token>& setOfTokens) {
		for (auto&& elem : setOfStrings)
			setOfTokens.insert(move(Token(elem, "char")));
	};
	convert(nonTerminals, mNonTerminals);
	convert(terminals, mTerminals);
	for (auto&& rule : rules) {
		auto currentToken = Token(rule.first, "char");
		auto& vectors = rule.second;
		vector<Token> temp;
		for (auto&& vec : vectors) {
			temp.clear();
			for (unsigned i = 0; i < vec.size(); i++) {
				if(!vec[i].empty()) temp.push_back(Token(vec[i], "char"));
			}
			if (temp.empty()) temp.push_back(Token("", "char"));
			mRules[currentToken].push_back(temp);
		}
	}
	removeDublicateRules(mRules);
	mAxiom = axiom;
}
CFG::CFG(const set<Token>& nonTerminals,
	const set<Token>& terminals,
	const ruleDict& rules,
	const Token& axiom)
	: mNonTerminals(nonTerminals), mTerminals(terminals), mAxiom(axiom) {
	//checks for input
	try {
		if (axiom.mLexem.empty()) {
			throw Exception("Lambda can't be an axiom");
		}
		if (!nonTerminals.contains(axiom))
			throw Exception("Non-terminals set doesn't have axiom.");
		for (auto& nt : nonTerminals)
			if (nt.mLexem.empty())
				throw Exception("Non-terminals has lambda token");
		for (auto& t : terminals)
			if (t.mLexem.empty())
				throw Exception("Terminals has lambda token");
		vector<Token> intersection;
		set_intersection(nonTerminals.begin(), nonTerminals.end(),
			terminals.begin(), terminals.end(), back_inserter(intersection));
		if (!intersection.empty())
			throw Exception("Terminal and Non-terminal sets intersect\n");
		ruleDict rulesCopy = rules;
		for (auto& rulePair : rulesCopy) {
			if (!nonTerminals.contains(rulePair.first))
				throw Exception("Rules have lhs non-terminal which is not in non-terminal set\n");
			for (auto& curRule : rulePair.second)
				for (unsigned i = 0; i < curRule.size(); i++)
				{
					if (!nonTerminals.contains(curRule[i]))
						if (!terminals.contains(curRule[i]) && !curRule[i].mLexem.empty())
							throw Exception("Symbol is not in non-terminals or terminals set\n");
				}
		}
	}
	catch (Exception e) {
		cerr << e.what();
		mAxiom = Token("S", "char");
		mNonTerminals.clear();
		mNonTerminals.insert(mAxiom);
		ruleRHS rRHS;
		vector<Token> temp; temp.push_back(Token("", "char"));
		rRHS.push_back(temp);
		mRules.clear();
		mRules[mAxiom] = rRHS;
		return;
	}
	for (auto&& rule : rules) {
		auto& currentToken = rule.first;
		auto& vectors = rule.second;
		vector<Token> temp;
		for (auto&& vec : vectors) {
			temp.clear();
			for (unsigned i = 0; i < vec.size(); i++) {
				if (!vec[i].mLexem.empty()) temp.push_back(vec[i]);
			}
			if (temp.empty()) temp.push_back(Token("", "char"));
			mRules[currentToken].push_back(temp);
		}
	}
	removeDublicateRules(mRules);
}
CFG::CFG(const CFG & other)
	: mNonTerminals(other.mNonTerminals), mTerminals(other.mTerminals),
	mRules(other.mRules), mAxiom(other.mAxiom) {}
CFG::CFG() {
	(*this) = emptyLanguage();
}

CFG& CFG::operator=(const CFG& other) noexcept
{
	if (this == &other)
		return *this;
	mNonTerminals = other.mNonTerminals;
	mTerminals = other.mTerminals;
	mRules = other.mRules;
	mAxiom = other.mAxiom;
	return *this;
}

CFG& CFG::operator=(CFG&& other) noexcept {
	mNonTerminals = move(other.mNonTerminals);
	mTerminals = move(other.mTerminals);
	mRules = move(other.mRules);
	mAxiom = move(other.mAxiom);
	return *this;
}

void CFG::printCFG() const noexcept
{
	cout << string(18,'-') << "\n";
	auto printSet = [&](const set<Token>& setOfTokens) {
		for (auto&& setElem : setOfTokens)
			cout << setElem.mLexem << " ";
		cout << "\n";
	};
	cout << "Non-terminals: ";
	printSet(mNonTerminals);
	cout << "Terminals: ";
	printSet(mTerminals);
	cout << "Rules:\n";
	for (auto&& rulePair : mRules) {
		cout << rulePair.first.mLexem << " : ";
		for (auto&& rhs : rulePair.second) {
			for (unsigned i = 0; i < rhs.size(); i++)
					cout << rhs[i].mLexem;
			cout << " | ";
		}
		cout << "\n";
	}

	cout << "Axiom: " << mAxiom.mLexem << endl;
}

bool CFG::operator==(const CFG& other) const noexcept{
	if (mAxiom == other.mAxiom)
		if (mNonTerminals == other.mNonTerminals)
			if (mTerminals == other.mTerminals)
			{
				set<Token> mKeys;
				for (auto& pair : mRules) {
					if (other.mRules.contains(pair.first)) {
						mKeys.insert(pair.first);
					}
					else
						return false;
				}
				ruleDict rRules = other.mRules;
				ruleDict lRules = mRules;
				for (auto& lhs : mKeys) {
					sort(rRules[lhs].begin(), rRules[lhs].end());
					sort(lRules[lhs].begin(), lRules[lhs].end());
					if (lRules[lhs] != rRules[lhs])
						return false;
				}
				return true;
			}
	return false;
}

bool CFG::isLanguageEmpty() const 
{
	return !getGoodNonTerminals().contains(mAxiom);
}

CFG CFG::emptyLanguage() const 
{
	Token axiom("S", "char");
	set<Token> nT; nT.insert(axiom);
	ruleDict rls;
	rls[axiom].push_back({Token("","char")});
	return CFG(nT, set<Token>(), rls, axiom);
}

CFG CFG::removeUnreachableSymbols() const
{
	set<Token> reachableNonTerminals; reachableNonTerminals.insert(mAxiom);
	set<Token> reachableNonTerminalsTemp;
	set<Token> reachableTerminals;
	do {
		reachableNonTerminalsTemp = reachableNonTerminals;
		for (auto& rulePair : mRules) {
			if (reachableNonTerminals.contains(rulePair.first))
				for (auto& rhs : rulePair.second)
					for (auto& tkn : rhs)
						if (mTerminals.contains(tkn))
							reachableTerminals.insert(tkn);
						else if (mNonTerminals.contains(tkn))
							reachableNonTerminals.insert(tkn);
		}
	} while (reachableNonTerminals != reachableNonTerminalsTemp);
	if (reachableTerminals == mTerminals && reachableNonTerminals == mNonTerminals)
		return CFG(*this);
	ruleDict newRules;
	for (auto& rulePair : mRules)
		if (reachableNonTerminals.contains(rulePair.first))
			newRules[rulePair.first] = rulePair.second;
	return CFG(move(reachableNonTerminals), move(reachableTerminals), move(newRules), mAxiom);
}

bool CFG::isRuleContainOnlyGoodTokens(
	const vector<Token>&rhs, 
	const set<Token>& goodTokens) const 
{
	if (rhs.empty()) return false;
	for (auto& token : rhs)
		if (!mTerminals.contains(token) && !goodTokens.contains(token))
			return false;
	return true;
}

set<Token> CFG::getLambdaNonTerminals() const  
{
	set<Token> lambdaNonTerminals = set<Token>();
	set<Token> lambdaNonTerminalsTemp = set<Token>();
	auto onlyLambadaNT = [&](const vector<Token>& rhs) {
		for (const auto& token : rhs)
			if (!lambdaNonTerminals.contains(token))
				return false;
		return true;
	};
	do {
		lambdaNonTerminalsTemp = lambdaNonTerminals;
		for ( const auto& rulePair : mRules) {
			for (const auto& rhs : rulePair.second) {
				if (onlyLambadaNT(rhs))
					lambdaNonTerminals.insert(rulePair.first);
				else if (rhs[0].mLexem.empty())
					lambdaNonTerminals.insert(rulePair.first);
			}
		}
	} while (lambdaNonTerminals != lambdaNonTerminalsTemp);
	return lambdaNonTerminals;
}

CFG CFG::removeBadNonTerminalsAndRules() const
{
	if (isLanguageEmpty())
		return emptyLanguage();
	auto goodNonTerminals = getGoodNonTerminals();
	ruleDict rulesCopy = mRules;
	for (auto& rulePair : mRules) {
		if (!goodNonTerminals.contains(rulePair.first))
			rulesCopy.erase(rulePair.first);
		else {
			ruleRHS ruleBuffer;
			for (auto& rhs : rulePair.second)
				if (isRuleContainOnlyGoodTokens(rhs, goodNonTerminals) || rhs[0].mLexem.empty())
					ruleBuffer.push_back(rhs);
			rulesCopy[rulePair.first] = ruleBuffer;
		}
	}
	return CFG(goodNonTerminals,mTerminals,rulesCopy,mAxiom);
}

void CFG::recursivePushBack(ruleRHS& result,
	vector<Token> tempChain,
	const vector<Token>& chain,
	const unsigned& adjPoint,
	const set<Token>& lambdaNT,
	const set<Token>& terminalNT) const 
{
	for (unsigned j = adjPoint + 1; j < chain.size(); ++j) {
		if(!lambdaNT.contains(chain[j]))
			tempChain.push_back(chain[j]);
		else {
			recursivePushBack(result, tempChain, chain, j, lambdaNT, terminalNT);
			if (terminalNT.contains(chain[j])) {
				tempChain.push_back(chain[j]);
				recursivePushBack(result, tempChain, chain, j, lambdaNT, terminalNT);
			}
			break;
		}
	}
	if (!tempChain.empty())
		result.push_back(move(tempChain));
}

void CFG::removeDublicateRules(ruleDict& rules) 
{
	for (auto& rulePair : rules) {
		sort(rulePair.second.begin(), rulePair.second.end());
		rulePair.second.erase(
			unique(rulePair.second.begin(), rulePair.second.end()),
			rulePair.second.end());
	}
}

CFG CFG::removeLambdaRules() const 
{ 
	const set<Token> lambdaNonTerminals = getLambdaNonTerminals();
	const set<Token> nonTerminalsTerminalChain = getTerminalNTForLambda(lambdaNonTerminals);
	ruleDict  rulesWithLambdaNonTerminals;
	ruleDict rulesWLNTBuffer;
	auto onlyLambdaCheck = [&](const vector<Token>& curChain) {
		for (const auto& token : curChain) 
			if (lambdaNonTerminals.contains(token))
				if (nonTerminalsTerminalChain.contains(token))
					return false;		
		return true;
	};
	auto doesHaveLambda = [&](const vector<Token>& curChain) {
		if (curChain[0].mLexem.empty())
			return true;
		for (const auto& token : curChain)
			if (lambdaNonTerminals.contains(token))
				return true;
		return false;
	};
	for (const auto& rulePair : mRules) 
		for (const auto& rhs : rulePair.second) 
			for (const auto& token : rhs) 
				if (lambdaNonTerminals.contains(token)) {
					rulesWithLambdaNonTerminals[rulePair.first].emplace_back(rhs);
					break;
				}

	vector<Token> temp;
	rulesWLNTBuffer = rulesWithLambdaNonTerminals;
	for (const auto& rulePair : rulesWLNTBuffer) {
		ruleRHS result;
		for (const auto& curChain : rulePair.second) {
			temp.clear();
			if (!onlyLambdaCheck(curChain)) {
				unsigned i = 0;
				while (!lambdaNonTerminals.contains(curChain[i]))
					temp.push_back(curChain[i++]);
				recursivePushBack(result, temp, curChain, i,lambdaNonTerminals,nonTerminalsTerminalChain);
				if (nonTerminalsTerminalChain.contains(curChain[i])) {
					temp.push_back(curChain[i]);
					recursivePushBack(result, temp, curChain, i, lambdaNonTerminals, nonTerminalsTerminalChain);
				}
			}
			else {
				for (auto i = 0; i < curChain.size(); ++i) {
					if (!lambdaNonTerminals.contains(curChain[i]))
						temp.push_back(curChain[i]);
				}
				if (!temp.empty())
					result.push_back(temp);
			}
		}
		if (!result.empty()) rulesWithLambdaNonTerminals[rulePair.first] = result;
		else rulesWithLambdaNonTerminals.erase(rulePair.first);
	}
	for (const auto& rulePair : mRules) {
		for (const auto& rhs : rulePair.second) {
			if (!doesHaveLambda(rhs)) {
				rulesWithLambdaNonTerminals[rulePair.first].emplace_back(rhs);
			}
		}
	}
	set<Token> finalNonTerminals = mNonTerminals;
	//for (const auto& pairRule : rulesWithLambdaNonTerminals)
	//	finalNonTerminals.insert(pairRule.first);
	//finalNonTerminals.insert(mNonTerminals.begin(), mNonTerminals.end());

	if (lambdaNonTerminals.contains(mAxiom)) {
		ruleRHS newAxiomRules;
		vector<Token> temp;
		temp.push_back(mAxiom);
		newAxiomRules.emplace_back(temp);
		temp.clear();
		temp.push_back(Token("", "char"));
		newAxiomRules.emplace_back(temp);
		Token newAxiom(mAxiom.mLexem + '\'', mAxiom.mLexemType);
		rulesWithLambdaNonTerminals[newAxiom] = newAxiomRules;
		finalNonTerminals.insert(newAxiom);
		return CFG(move(finalNonTerminals), mTerminals, move(rulesWithLambdaNonTerminals), move(newAxiom));
	}
	return CFG(move(finalNonTerminals),mTerminals,move(rulesWithLambdaNonTerminals),mAxiom);
}

CFG CFG::removeLeftRecursion() const 
{
	if (isLanguageEmpty())
		return emptyLanguage();
	CFG newGrammar = removeChainRules();
	vector<Token> orderedTokens;
	orderedTokens.assign(newGrammar.mNonTerminals.begin(), newGrammar.mNonTerminals.end());
	unsigned i, j;
	i = 0;
	do {
		Token currentNonTerminal = orderedTokens[i];
		for (auto& rhs : newGrammar.mRules[currentNonTerminal])
			if (rhs[0] == currentNonTerminal) {
				Token newNonTerminal(currentNonTerminal.mLexem + "\'",currentNonTerminal.mLexemType);
				ruleRHS newNonTerminalRules;
				ruleRHS noRecursionRules;
				for (auto& currentRule : newGrammar.mRules[currentNonTerminal]) {
					if (currentRule[0] == currentNonTerminal) {
						vector<Token> newRule(currentRule.begin()+1,currentRule.end());
						newNonTerminalRules.push_back(newRule);
						newRule.push_back(newNonTerminal);
						newNonTerminalRules.push_back(newRule);
					}
					else {
						noRecursionRules.push_back(currentRule);
						vector<Token> temp = currentRule;
						temp.push_back(newNonTerminal);
						noRecursionRules.push_back(temp);
					}
				}
				newGrammar.mRules[currentNonTerminal] = noRecursionRules;
				newGrammar.mNonTerminals.insert(newNonTerminal);
				newGrammar.mRules[newNonTerminal] = newNonTerminalRules;
				break;
			}
		if (i == orderedTokens.size()-1)
			break;
		j = -1;
		i++;
		do {
			j++;
			ruleRHS tempRules;
			for (auto& currentRule : newGrammar.mRules[orderedTokens[i]]) {
				if (currentRule[0] == orderedTokens[j]) {
					for (auto& prevNonTerminalRule : newGrammar.mRules[orderedTokens[j]]) {
						vector<Token> tempChain(prevNonTerminalRule);
						tempChain.insert(tempChain.end(), currentRule.begin() + 1, currentRule.end());
						tempRules.push_back(tempChain);
					}
				}
				else {
					tempRules.push_back(currentRule);
				}
			}
			newGrammar.mRules[orderedTokens[i]] = tempRules;
		} while (j != i - 1);
	} while (true);
	return newGrammar; //убрал removeChainRules и removeUselessSymbols
}

CFG CFG::removeChainRules() const
{
	CFG newGrammar = removeLambdaRules();
	auto hasOneNonTerminal = [&](const vector<Token>& ruleOutput) ->  bool { return (ruleOutput.size() == 1 && newGrammar.mNonTerminals.contains(ruleOutput[0])); };
	auto hasNotOneNonTerminal = [&](const vector<Token>& ruleOutput) ->  bool { return !(ruleOutput.size() == 1 && newGrammar.mNonTerminals.contains(ruleOutput[0])); };
	
	map<Token, set<Token>> chainNonTerminalsSets;
	for (const auto& firstNonTerminalInSet : newGrammar.mNonTerminals) {
		set<Token> chainNonTerminalsInSet;
		chainNonTerminalsInSet.insert(firstNonTerminalInSet); // я тупанул, и когда реализовывал на питоне не сделал так, из-за чего там есть лишний код
		set<Token> chainNonTerminalsInSetTemp(chainNonTerminalsInSet);
		do {
			for (const auto& chainNonTerminal : chainNonTerminalsInSetTemp) {
				for (const auto& ruleOutput : newGrammar.mRules[chainNonTerminal]
					| views::filter(hasOneNonTerminal)) {
					chainNonTerminalsInSetTemp.insert(ruleOutput[0]);
				}
			}
			
			set<Token> setDifference;
			set_difference(chainNonTerminalsInSetTemp.begin(), chainNonTerminalsInSetTemp.end(), chainNonTerminalsInSet.begin(), chainNonTerminalsInSet.end(),
				std::inserter(setDifference, setDifference.end()));
			chainNonTerminalsInSetTemp = move(setDifference);

			if (!chainNonTerminalsInSetTemp.empty()) {
				set<Token> setUnion;
				set_union(chainNonTerminalsInSet.begin(), chainNonTerminalsInSet.end(), chainNonTerminalsInSetTemp.begin(), chainNonTerminalsInSetTemp.end(),
					std::inserter(setUnion, setUnion.end()));
				chainNonTerminalsInSet = move(setUnion);
			}
		} while (!chainNonTerminalsInSetTemp.empty());

		chainNonTerminalsSets[firstNonTerminalInSet] = chainNonTerminalsInSet;
	}

	ruleDict newRules;
	for (const auto& chainNonTerminalsSet : chainNonTerminalsSets) {
		ruleRHS ruleRHSForChainNonTerminalSet;
		for (const auto& nonTerminal : chainNonTerminalsSet.second) {
			copy_if(newGrammar.mRules[nonTerminal].begin(), newGrammar.mRules[nonTerminal].end(), back_inserter(ruleRHSForChainNonTerminalSet), hasNotOneNonTerminal);
		}

		newRules[chainNonTerminalsSet.first] = move(ruleRHSForChainNonTerminalSet);
	}

	newGrammar.mRules = move(newRules);
	newGrammar = newGrammar.removeUnreachableSymbols();
	newGrammar.mTerminals = mTerminals;
	return newGrammar;
}

CFG CFG::makeChomskyNormalForm() const
{
	//1) добавить правила A->a, a is in T
	//2) добавить правила A->BC, BC are in NT
	//3) добавить правила S->lambda
	//4) для правил A->X1X2X3...Xn   //n>2
	//      производим замену A->X1'<X2...Xn>
	//                   then <X2...Xn> -> X2'<X3...Xn>
	//                         <X_k-1X_k> -> X'_k-1X'_k
	//X' = X, if X is NT
	//else X'=X', and add rule X'->X
	//5) обработать правила n=2
	//   A->aB-> a'B, A->Ba ->Ba', A->ab ->a'b'
	//6) для новых нетерминалов a'(a is in T) add rule a'->a
	CFG newGrammar = removeLambdaRules();
	ruleDict newRules;
	pair<size_t, size_t> combination= {0, 1};
	set<pair<Token,vector<Token>>> newNonTerminals;
	auto makeNewRulesofSize2 = [&](const pair<size_t, size_t>& comb,
		const vector<Token>& currentRule,
		const Token& ruleLHS) {
		// comb = {0,1} -> first=nonTerminal, second-terminal;
		vector<Token> tempRule;
		Token newNonTerminal(currentRule[comb.second].mLexem + "\'", currentRule[comb.second].mLexemType);
		tempRule.push_back(currentRule[comb.first]);
		tempRule.push_back(newNonTerminal);
		if (comb.first > comb.second) reverse(tempRule.begin(), tempRule.end());
		vector<Token> terminalRule; terminalRule.push_back(currentRule[comb.second]);
		newRules[ruleLHS].push_back(tempRule);
		newGrammar.mNonTerminals.insert(newNonTerminal);
		newNonTerminals.insert(make_pair<Token, vector<Token>>(move(newNonTerminal), move(terminalRule)));
	};
	auto makeNewRulesOfTwoTerminals = [&](const pair<size_t, size_t>& comb,
		const vector<Token>& currentRule,
		const Token& ruleLHS) {
			vector<Token> tempRule; vector<Token> firstRule; vector<Token> secondRule;
			Token firstNT = Token(currentRule[comb.first].mLexem + "\'", currentRule[comb.first].mLexemType); firstRule.push_back(currentRule[0]);
			Token secondNT = Token(currentRule[comb.second].mLexem + "\'", currentRule[comb.second].mLexemType); secondRule.push_back(currentRule[1]);
			tempRule.push_back(firstNT); tempRule.push_back(secondNT);
			newRules[ruleLHS].push_back(tempRule);
			newGrammar.mNonTerminals.insert(firstNT);
			newGrammar.mNonTerminals.insert(secondNT);
			newNonTerminals.insert(make_pair<Token, vector<Token>>(move(firstNT), move(firstRule)));
			newNonTerminals.insert(make_pair<Token, vector<Token>>(move(secondNT), move(secondRule)));
	};
	for (auto& rulePair : newGrammar.mRules)
		for (auto& currentRule : rulePair.second) {
			switch (currentRule.size()) {
			case 1:
				newRules[rulePair.first].push_back(currentRule);
				break;
			case 2:
				if (newGrammar.mNonTerminals.contains(currentRule[0])) // rules AB or Ab
				{
					if (newGrammar.mNonTerminals.contains(currentRule[1])) {
						newRules[rulePair.first].push_back(currentRule);
					}
					else {
						combination = { 0,1 };
						makeNewRulesofSize2(combination, currentRule, rulePair.first);
					}
				}
				else { //rules aB or aa
					if (newGrammar.mNonTerminals.contains(currentRule[1])) {
						combination = { 1,0 };
						makeNewRulesofSize2(combination, currentRule, rulePair.first);
					}
					else {
						combination = { 0, 1 };
						makeNewRulesOfTwoTerminals(combination, currentRule, rulePair.first);
					}
				}
				break;
			default:
				Token newNonTerminal;
				Token newGroupNonTerminal;
				vector<Token> ruleChain;
				string groupLexem;
				unsigned i = 0;
				Token currentLHS = rulePair.first;
				do {
					ruleChain.clear();
					groupLexem = "<";
					if (newGrammar.mTerminals.contains(currentRule[i]))
					{
						newNonTerminal = Token(currentRule[i].mLexem + "\'", currentRule[i].mLexemType);
						vector<Token> temp; temp.push_back(currentRule[i]);
						newNonTerminals.insert(make_pair<Token, vector<Token>>(Token(newNonTerminal), move(temp)));
						newGrammar.mNonTerminals.insert(newNonTerminal);
					}
					else
						newNonTerminal = currentRule[i];
					for (unsigned j = i + 1; j < currentRule.size(); j++) {
						groupLexem += currentRule[j].mLexem;
					}
					groupLexem += ">";
					newGroupNonTerminal = Token(groupLexem, "char");
					newGrammar.mNonTerminals.insert(newGroupNonTerminal);
					ruleChain.push_back(newNonTerminal); ruleChain.push_back(newGroupNonTerminal);
					newRules[currentLHS].push_back(ruleChain);
					currentLHS = newGroupNonTerminal;
				} while (++i < currentRule.size() - 2);
				//case 2 copy_paste :/, idk maybe lambda=func would solve this problem but 
				//1)i dont care 
				//2)didn't read 
				//3)idgaf 
				//4)i have copy->paste binds
				//оставил этот коммент для тебя, Евгений)) потому что по итогу написал лямбды 😡
				Token latestToken = currentRule[currentRule.size() - 1];
				Token penultToken = currentRule[currentRule.size() - 2];
				if (newGrammar.mNonTerminals.contains(penultToken)) // rules AB or Ab
				{
					if (newGrammar.mNonTerminals.contains(latestToken))
						newRules[currentLHS].push_back(vector<Token>(currentRule.begin() + i, currentRule.end()));
					else {
						combination = { currentRule.size() - 2,currentRule.size() - 1 };
						makeNewRulesofSize2(combination, currentRule, currentLHS);
					}
				}
				else { //rules aB or aa
					if (newGrammar.mNonTerminals.contains(latestToken)) {
						combination = { currentRule.size() - 1,currentRule.size() - 2 };
						makeNewRulesofSize2(combination, currentRule, currentLHS);
					}
					else {
						combination = { currentRule.size() - 2,currentRule.size() - 1 };
						makeNewRulesOfTwoTerminals(combination, currentRule, currentLHS);
					}
				}
				break;
			}
		}
		for (auto&& rulePair : newNonTerminals)
			newRules[rulePair.first].push_back(rulePair.second);
		return CFG(move(newGrammar.mNonTerminals), move(newGrammar.mTerminals), move(newRules), move(newGrammar.mAxiom));
}

CFG CFG::makeGreibachNormalForm() const
{
	if (isLanguageEmpty())
		return emptyLanguage();
	CFG newGrammar = removeLeftRecursion();
	vector<Token> orderedTokens(newGrammar.mNonTerminals.begin(),newGrammar.mNonTerminals.end());
	long long startFromTerminalIndex = orderedTokens.size()-1;
	for (auto& rulePair : newGrammar.mRules) {
		bool needToOrder = false;
		for (auto& rule : rulePair.second)
			if (newGrammar.mNonTerminals.contains(rule[0])) {
				int lhsPos;
				int rhsPos;
				for (int i = 0; i < orderedTokens.size(); i++) {
					if (orderedTokens[i] == rulePair.first)
						lhsPos = i;
					else if (orderedTokens[i] == rule[0])
						rhsPos = i;
				}
				if (lhsPos > rhsPos)
					swap(orderedTokens[lhsPos], orderedTokens[rhsPos]);
				needToOrder = true;
				break;
			}
		if (!needToOrder )iter_swap(find(orderedTokens.begin(), orderedTokens.end(), rulePair.first), 
			orderedTokens.begin() + startFromTerminalIndex);

	}
	long long i = orderedTokens.size() - 2;
	ruleDict newRules;
	newRules[orderedTokens[i + 1]] = newGrammar.mRules[orderedTokens[i + 1]];
	ruleRHS tempRules;
	while (i >= 0) {
		tempRules.clear();
		for (const auto& currentRule : newGrammar.mRules[orderedTokens[i]]) {
			bool firstTokenNT = false;
			long long j = i + 1;
			for (; j < orderedTokens.size() && !firstTokenNT; j++) {
				if (currentRule[0] == orderedTokens[j]) {
					firstTokenNT = true;
				}
			}
			j--;
			if (firstTokenNT) {
				vector<Token> ruleRHS(currentRule.begin()+1,currentRule.end());
				for (auto& firstTokenRulesRHS : newRules[orderedTokens[j]]){
					vector<Token> tempRHS=ruleRHS;
					tempRHS.insert(tempRHS.begin(), firstTokenRulesRHS.begin(), firstTokenRulesRHS.end());
					tempRules.push_back(move(tempRHS));
				}
			}
			else {
				tempRules.push_back(currentRule);
			}
		}
		newRules[orderedTokens[i]] = tempRules;
		i--;
	}
	set<pair<Token,vector<Token>>> newTerminalRules;
	for (const auto& rulePair : newRules) {
		tempRules.clear();
		for (const auto& currentRule : rulePair.second) {
			if (currentRule.size() > 1) {
				vector<Token> rightSideOfTerminal(currentRule.begin() + 1, currentRule.end());
				vector<Token> ruleConstructor; 
				ruleConstructor.push_back(currentRule[0]);
				for (auto& token : rightSideOfTerminal) {
					if (newGrammar.mNonTerminals.contains(token)) {
						ruleConstructor.push_back(token);
					}
					else {
						Token terminalToken = Token(token.mLexem + "\'", "char");
						vector<Token> terminalRule;
						terminalRule.push_back(token);
						ruleConstructor.push_back(terminalToken);
						newTerminalRules.insert(make_pair<Token, vector<Token>>(move(terminalToken), move(terminalRule)));
					}
				}
				tempRules.push_back(ruleConstructor);
			}
			else {
				tempRules.push_back(currentRule);
			}
		}
		newRules[rulePair.first] = tempRules;
	}
	for (const auto& terminalPair : newTerminalRules) {
		newGrammar.mNonTerminals.insert(terminalPair.first);
		newRules[terminalPair.first].push_back(terminalPair.second);
	}
	return CFG(move(newGrammar.mNonTerminals), move(newGrammar.mTerminals),move(newRules),move(newGrammar.mAxiom));
}

set<Token> CFG::getGoodNonTerminals() const 
{
	set<Token> goodTokens=set<Token>();
	set<Token> goodTokensTemp=set<Token>();
	do {
		goodTokensTemp = goodTokens;
		for (const auto& rulePair : mRules) {
			for (const auto& rhs : rulePair.second) {
				if (isRuleContainOnlyGoodTokens(rhs, goodTokens)) {
					goodTokens.insert(rulePair.first);
					break;
				}
			}
		}
	} while (goodTokens != goodTokensTemp);
	return goodTokens;
}

set<Token> CFG::getTerminalNTForLambda(const set<Token>& lambdaNT) const 
{
	set<Token> res = lambdaNT;
	set<Token> resultEnd;
	set<Token> ans;
	set<Token> goodNonTerminals=getGoodNonTerminals();
	do {
		resultEnd = res;
		for (const auto& rulePair : mRules) {
			for (const auto& rhs : rulePair.second) {
				if (isRuleContainOnlyGoodTokens(rhs, res)) {
					int terminalsCount = 0;
					for (const auto& token : rhs) {
						if (mTerminals.contains(token) || goodNonTerminals.contains(token))
							terminalsCount++;
					}
					if (terminalsCount != 0) {
						res.insert(rulePair.first);
						ans.insert(rulePair.first);
						break;
					}
				}
			}
		}
	} while (res != resultEnd);

	return ans;
}

CFG CFG::removeUselessSymbols() const
{
	if (isLanguageEmpty())
		return emptyLanguage();
	return removeBadNonTerminalsAndRules().removeUnreachableSymbols();
}

wstring CFG::toWString() const
{
	std::wstring result;
	auto printSet = [&](const set<Token>& setOfTokens) {
		for (auto&& setElem : setOfTokens)
			result = result + wstring(setElem.mLexem.begin(), setElem.mLexem.end()) + L" ";
		result += L"\n";
	};
	result += L"Non-terminals: ";
	printSet(mNonTerminals);
	result+= L"Terminals: ";
	printSet(mTerminals);
	result+=L"Rules:\n";
	for (auto&& rulePair : mRules) {
		result+=wstring(rulePair.first.mLexem.begin(),rulePair.first.mLexem.end()) + L" : ";
		for (auto&& rhs : rulePair.second) {
			for (unsigned i = 0; i < rhs.size(); i++)
				result+= wstring(rhs[i].mLexem.begin(),rhs[i].mLexem.end());
			result+= L" | ";
		}
		result+=L"\n";
	}

	result+=L"Axiom: " + wstring(mAxiom.mLexem.begin(),mAxiom.mLexem.end())+L"\n";
	return result;
}
