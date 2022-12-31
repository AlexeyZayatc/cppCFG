#include "CFG.h"
std::ostream& operator <<(std::ostream& ostr, const Token& token) {
	std::cout << '(' << token.mLexem << ',' << token.mLexemType << ')';
	return ostr;
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

void CFG::printCFG()
{
	cout << string(18,'-') << "\n";
	auto printSet = [&](set<Token> setOfTokens) {
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

bool CFG::isLanguageEmpty()
{
	return !getGoodNonTerminals().contains(mAxiom);
}

CFG CFG::emptyLanguage()
{
	set<Token> nT; nT.insert(mAxiom);
	ruleDict rls;
	rls[mAxiom].push_back({Token("","char")});
	return CFG(nT, set<Token>(), rls, mAxiom);
}

CFG CFG::removeUnreachableSymbols()
{
	set<Token> reachableNonTerminals; reachableNonTerminals.insert(mAxiom);
	set<Token> reachableNonTerminalsTemp = reachableNonTerminals;
	set<Token> reachableTerminals;
	do {
		reachableNonTerminalsTemp = reachableNonTerminals;
		for (auto& rulePair : mRules) {
			if (reachableNonTerminals.contains(rulePair.first))
				for (auto& rhs : rulePair.second)
					for (auto& tkn : rhs)
						if (mTerminals.contains(tkn))
							reachableTerminals.insert(tkn);
						else reachableNonTerminals.insert(tkn);
		}
	} while (reachableNonTerminals != reachableNonTerminalsTemp);
	if (reachableTerminals == mTerminals && reachableNonTerminals == mNonTerminals)
		return CFG(*this);
	ruleDict newRules;
	for (auto& rulePair : mRules)
		if (reachableNonTerminals.contains(rulePair.first)) 
			newRules[rulePair.first] = mRules[rulePair.first];
	return CFG(reachableNonTerminals, reachableTerminals, newRules, mAxiom);
}

bool CFG::isRuleContainOnlyGoodTokens(
	const vector<Token>&rhs, 
	const set<Token>& goodTokens) noexcept
{
	if (rhs.empty()) return false;
	for (auto& token : rhs)
		if (!mTerminals.contains(token) && !goodTokens.contains(token))
			return false;
	return true;
}

set<Token> CFG::getLambdaNonTerminals()
{
	set<Token> lambdaNonTerminals = set<Token>();
	set<Token> lambdaNonTerminalsTemp = set<Token>();
	auto onlyLambadaNT = [&](vector<Token> rhs) {
		for (auto& token : rhs)
			if (!lambdaNonTerminals.contains(token))
				return false;
		return true;
	};
	do {
		lambdaNonTerminalsTemp = lambdaNonTerminals;
		for (auto& rulePair : mRules) {
			for (auto& rhs : mRules[rulePair.first]) {
				if (rhs.empty())
					lambdaNonTerminals.insert(rulePair.first);
				else if (onlyLambadaNT(rhs))
					lambdaNonTerminals.insert(rulePair.first);
			}
		}
	} while (lambdaNonTerminals != lambdaNonTerminalsTemp);
	return lambdaNonTerminals;
}



CFG CFG::removeBadNonTerminalsAndRules()
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
				if (isRuleContainOnlyGoodTokens(rhs, goodNonTerminals))
					ruleBuffer.push_back(rhs);
			rulesCopy[rulePair.first] = ruleBuffer;
		}
	}
	return CFG(goodNonTerminals,mTerminals,rulesCopy,mAxiom);
}

void CFG::recursivePushBack(ruleRHS& result,vector<Token> tempChain,vector<Token> chain ,unsigned adjPoint)
{
	for (unsigned j = adjPoint + 1; j < chain.size(); ++j) {
		if(!getLambdaNonTerminals().contains(chain[j]))
			tempChain.push_back(chain[j]);
		else {
			recursivePushBack(result, tempChain, chain, j);
			if (getTerminalNTForLambda(getLambdaNonTerminals()).contains(chain[j])) {
				tempChain.push_back(chain[j]);
				recursivePushBack(result, tempChain, chain, j);
			}
			break;
		}
	}
	if (!tempChain.empty() && find(result.begin(), result.end(), tempChain) == result.end())
		result.push_back(tempChain);
}

CFG CFG::removeLambdaRules()
{ 
	set<Token> lambdaNonTerminals = getLambdaNonTerminals();
	set<Token> nonTerminalsTerminalChain = getTerminalNTForLambda(lambdaNonTerminals);
	ruleDict  rulesWithLambdaNonTerminals;
	ruleDict rulesWLNTBuffer;
	auto onlyLambdaCheck = [&](vector<Token> curChain) {
		for (auto& token : curChain) 
			if (lambdaNonTerminals.contains(token))
				if (nonTerminalsTerminalChain.contains(token))
					return false;		
		return true;
	};
	auto doesHaveLambda = [&](vector<Token> curChain) {
		if (curChain.empty())
			return true;
		for (auto& token : curChain)
			if (lambdaNonTerminals.contains(token))
				return true;
		return false;
	};
	for (auto& rulePair : mRules) {
		for (auto& rhs : mRules[rulePair.first]) {

			for (auto& token : rhs) {
				if (lambdaNonTerminals.contains(token)) {
					rulesWithLambdaNonTerminals[rulePair.first].emplace_back(rhs);
					break;
				}
			}
		}
	}
	vector<Token> temp;
	rulesWLNTBuffer = rulesWithLambdaNonTerminals;
	for (auto& rulePair : rulesWLNTBuffer) {
		ruleRHS result;
		for (auto& curChain : rulesWithLambdaNonTerminals[rulePair.first]) {
			temp.clear();
			if (!onlyLambdaCheck(curChain)) {
				unsigned i = 0;
				while (!lambdaNonTerminals.contains(curChain[i]))
					temp.push_back(curChain[i++]);
				recursivePushBack(result, temp, curChain, i);
				if (nonTerminalsTerminalChain.contains(curChain[i])) {
					temp.push_back(curChain[i]);
					recursivePushBack(result, temp, curChain, i);
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
	for (auto& rulePair : mRules) {
		for (auto& rhs : mRules[rulePair.first]) {
			if (!doesHaveLambda(rhs)) {
				rulesWithLambdaNonTerminals[rulePair.first].emplace_back(rhs);
			}
		}
	}
	set<Token> finalNonTerminals;
	for (auto& pairRule : rulesWithLambdaNonTerminals)
		finalNonTerminals.insert(pairRule.first);

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
		return CFG(finalNonTerminals, mTerminals, rulesWithLambdaNonTerminals, newAxiom);
	}
	return CFG(finalNonTerminals,mTerminals,rulesWithLambdaNonTerminals,mAxiom);
}

set<Token> CFG::getGoodNonTerminals()
{
	set<Token> goodTokens=set<Token>();
	set<Token> goodTokensTemp=set<Token>();
	do {
		goodTokensTemp = goodTokens;
		for (auto& rulePair : mRules) {
			for (auto& rhs : mRules[rulePair.first]) {
				if (isRuleContainOnlyGoodTokens(rhs, goodTokens)) {
					goodTokens.insert(rulePair.first);
					break;
				}
			}
		}
	} while (goodTokens != goodTokensTemp);
	return goodTokens;
}

set<Token> CFG::getTerminalNTForLambda(set<Token> lambdaNT)
{
	set<Token> res = set<Token>();
	set<Token> resultEnd;
	set<Token> ans;
	res.merge(lambdaNT);
	res.merge(getGoodNonTerminals());
	do {
		resultEnd = res;
		for (auto& rulePair : mRules) {
			for (auto& rhs : mRules[rulePair.first]) {
				if (isRuleContainOnlyGoodTokens(rhs, res)) {
					int terminalsCount = 0;
					for (auto& token : rhs) {
						if (mTerminals.contains(token) || getGoodNonTerminals().contains(token))
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

	return set<Token>(ans);
}

CFG CFG::removeUselessSymbols()
{
	if (isLanguageEmpty())
		return emptyLanguage();
	return removeBadNonTerminalsAndRules().removeUnreachableSymbols();
}

wstring CFG::toWString() const
{
	std::wstring result;
	auto printSet = [&](set<Token> setOfTokens) {
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
		cout << rulePair.first.mLexem << " : ";
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
