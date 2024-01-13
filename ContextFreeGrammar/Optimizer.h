#pragma once
#include <set>

#include "Parser.h"
#include "CLexems.h"

class Optimizer
{
public:
	Optimizer(Node* _start);
	void optimizeCode();
private:
	void deleteUnusedVars1();
	void recursiveTraverse(Node* current, Node* parent, std::map<std::string, size_t>& namesCount);
	bool recursiveVarDelete(Node* current, Node* parent, std::map<std::string, size_t>& namesCount);
	void deleteUnusedVars2();
	void deleteUnusedVarsRecursive(Node* current, std::map<std::string, size_t>& namesCount, bool& found);

	const std::set<std::string> wrongParents = { "NodeDeclarator", "NodeAssigning", "NodeInitDeclarator"};
	Node* start;
};

