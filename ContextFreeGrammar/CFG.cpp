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
		for (auto& nt : nonTerminals)
			for (auto& t : terminals)
				if (nt == t)
					throw Exception("Terminal and Non-terminal sets intersect\n");
		map<string, vector<string>> rulesCopy = rules;
		std::string cursubStr;
		for (auto& rulePair : rulesCopy) {
			if (!nonTerminals.contains(rulePair.first))
				throw Exception("Rules have lhs non-terminal which is not in non-terminal set\n");
			for (auto& curRule : rulesCopy[rulePair.first])
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
	auto convert = [&](set<string> setOfStrings, set<Token>& setOfTokens) {
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
		for (auto& nt : nonTerminals)
			for (auto& t : terminals)
				if (nt == t)
					throw Exception("Terminal and Non-terminal sets intersect\n");
		map<string, vector<vector<string>>> rulesCopy = rules;
		for (auto& rulePair : rulesCopy) {
			if (!nonTerminals.contains(rulePair.first))
				throw Exception("Rules have lhs non-terminal which is not in non-terminal set\n");
			for (auto& curRule : rulesCopy[rulePair.first])
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
	auto convert = [&](set<string> setOfStrings, set<Token>& setOfTokens) {
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
		for (auto& nt : nonTerminals)
			for (auto& t : terminals)
				if (nt == t)
					throw Exception("Terminal and Non-terminal sets intersect");
		ruleDict rulesCopy = rules;
		for (auto& rulePair : rulesCopy) {
			if (!nonTerminals.contains(rulePair.first))
				throw Exception("Rules have lhs non-terminal which is not in non-terminal set\n");
			for (auto& curRule : rulesCopy[rulePair.first])
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
	Token axiom("S", "char");
	set<Token> nT; nT.insert(axiom);
	ruleDict rls;
	rls[axiom].push_back({Token("","char")});
	return CFG(nT, set<Token>(), rls, axiom);
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
				if (onlyLambadaNT(rhs))
					lambdaNonTerminals.insert(rulePair.first);
				else if (rhs[0].mLexem.empty())
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
				if (isRuleContainOnlyGoodTokens(rhs, goodNonTerminals) || rhs[0].mLexem.empty())
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

void CFG::removeDublicateRules(ruleDict& rules)
{
	for (auto& rulePair : rules) {
		sort(rules[rulePair.first].begin(), rules[rulePair.first].end());
		rules[rulePair.first].erase(
			unique(rules[rulePair.first].begin(), rules[rulePair.first].end()),
				rules[rulePair.first].end());
	}
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
		if (curChain[0].mLexem.empty())
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

CFG CFG::removeLeftRecursion()
{
	if (isLanguageEmpty())
		return emptyLanguage();
	CFG newGrammar = removeLambdaRules().removeUselessSymbols();//TODO: removeChainRules;
	vector<Token> orderedTokens;
	for (auto& nonTerminal : mNonTerminals)
		orderedTokens.push_back(nonTerminal);
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
	return newGrammar.removeUselessSymbols();//TODO: remove chain rules too!
}

CFG CFG::makeChomskyNormalForm()
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
	//for (auto& axiomRule : mRules[mAxiom])
	//	if (axiomRule[0] == Token("", "char") && axiomRule.size() == 1){
	//		newRules[mAxiom].push_back(axiomRule);
	//		break;
	//	}

	for (auto& rulePair : newGrammar.mRules)
		for (auto& currentRule : newGrammar.mRules[rulePair.first]) {
			switch (currentRule.size()) {
			case 1:
				newRules[rulePair.first].push_back(currentRule);
				break;
			case 2:
				if (newGrammar.mNonTerminals.contains(currentRule[0])) // rules AB or Ab
				{
					if(newGrammar.mNonTerminals.contains(currentRule[1]))
						newRules[rulePair.first].push_back(currentRule);
					else {
						vector<Token> tempRule = currentRule;
						tempRule.pop_back();
						Token newNonTerminal(currentRule[1].mLexem+"\'", currentRule[1].mLexemType);
						tempRule.push_back(newNonTerminal);
						vector<Token> terminalRule; terminalRule.push_back(currentRule[1]);
						newRules[newNonTerminal].push_back(terminalRule);
						newRules[rulePair.first].push_back(tempRule);
						newGrammar.mNonTerminals.insert(newNonTerminal);
					}
				}
				else { //rules aB or aa
					if (newGrammar.mNonTerminals.contains(currentRule[1])) {
						vector<Token> tempRule;
						Token newNonTerminal(currentRule[0].mLexem + "\'", currentRule[1].mLexemType);
						tempRule.push_back(newNonTerminal);
						tempRule.push_back(currentRule[1]);
						vector<Token> terminalRule; terminalRule.push_back(currentRule[0]);
						newRules[newNonTerminal].push_back(terminalRule);
						newRules[rulePair.first].push_back(tempRule);
						newGrammar.mNonTerminals.insert(newNonTerminal);
					}
					else {
						vector<Token> tempRule; vector<Token> firstRule; vector<Token> secondRule;
						Token firstNT = Token(currentRule[0].mLexem + "\'", currentRule[0].mLexemType); firstRule.push_back(currentRule[0]);
						Token secondNT = Token(currentRule[1].mLexem + "\'", currentRule[1].mLexemType); secondRule.push_back(currentRule[1]);
						tempRule.push_back(firstNT); tempRule.push_back(secondNT);
						newRules[rulePair.first].push_back(tempRule);
						newRules[firstNT].push_back(firstRule);
						newRules[secondNT].push_back(secondRule);
						newGrammar.mNonTerminals.insert(firstNT);
						newGrammar.mNonTerminals.insert(secondNT);
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
				do  {
					ruleChain.clear();
					groupLexem = "<";
					if (newGrammar.mTerminals.contains(currentRule[i]))
					{
						newNonTerminal = Token(currentRule[i].mLexem + "\'", currentRule[i].mLexemType);
						vector<Token> temp; temp.push_back(currentRule[i]);
						newRules[newNonTerminal].push_back(temp);
						newGrammar.mNonTerminals.insert(newNonTerminal);
					}
					else
						newNonTerminal = currentRule[i];
					for (unsigned j = i+1; j < currentRule.size(); j++) {
						groupLexem += currentRule[j].mLexem;
					}
					groupLexem += ">";
					newGroupNonTerminal = Token(groupLexem, "char");
					newGrammar.mNonTerminals.insert(newGroupNonTerminal);
					ruleChain.push_back(newNonTerminal); ruleChain.push_back(newGroupNonTerminal);
					newRules[currentLHS].push_back(ruleChain);
					currentLHS = newGroupNonTerminal;
				} while (++i < currentRule.size()-2);
				//case 2 copy_paste :/, idk maybe lambda=func would solve this problem but 
				//1)i dont care 
				//2)didn't read 
				//3)idgaf 
				//4)i have copy->paste binds
				Token latestToken = currentRule[currentRule.size() - 1];
				Token penultToken = currentRule[currentRule.size() - 2];
				if (newGrammar.mNonTerminals.contains(penultToken)) // rules AB or Ab
				{
					if (newGrammar.mNonTerminals.contains(latestToken))
						newRules[currentLHS].push_back(vector<Token>(currentRule.begin()+i,currentRule.end()));
					else {
						vector<Token> tempRule = {penultToken};
						Token newNonTerminal(latestToken.mLexem + "\'", latestToken.mLexemType);//b'
						newGrammar.mNonTerminals.insert(newNonTerminal);
						tempRule.push_back(newNonTerminal); //b'
						vector<Token> terminalRule; terminalRule.push_back(latestToken); //->b
						newRules[newNonTerminal].push_back(terminalRule);//b'->b
						newRules[currentLHS].push_back(tempRule);//<Ab>->Ab'
					}
				}
				else { //rules aB or aa
					if (newGrammar.mNonTerminals.contains(latestToken)) {
						vector<Token> tempRule;
						Token newNonTerminal(penultToken.mLexem + "\'", latestToken.mLexemType);//a'
						newGrammar.mNonTerminals.insert(newNonTerminal);
						tempRule.push_back(newNonTerminal);//->a'
						tempRule.push_back(latestToken);//->a'B
						vector<Token> terminalRule; terminalRule.push_back(penultToken); //->a
						newRules[newNonTerminal].push_back(terminalRule);//a'->a
						newRules[currentLHS].push_back(tempRule);//<aB>->a'B
					}
					else {
						vector<Token> tempRule; vector<Token> firstRule; vector<Token> secondRule;
						Token firstNT = Token(penultToken.mLexem + "\'", penultToken.mLexemType);//a'
						firstRule.push_back(penultToken);//->a
						Token secondNT = Token(latestToken.mLexem + "\'", latestToken.mLexemType);//b'
						secondRule.push_back(latestToken);//->b
						tempRule.push_back(firstNT); tempRule.push_back(secondNT);//->a'b'
						newRules[currentLHS].push_back(tempRule);//<ab>->a'b'
						newRules[firstNT].push_back(firstRule);//a'->a
						newRules[secondNT].push_back(secondRule);//b'->b
						newGrammar.mNonTerminals.insert(firstNT);
						newGrammar.mNonTerminals.insert(secondNT);
					}
				}
				break;
			}
		}
		removeDublicateRules(newRules);
	return CFG(newGrammar.mNonTerminals,newGrammar.mTerminals,newRules,newGrammar.mAxiom); // TODO: MAYBE remove chain rules
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
