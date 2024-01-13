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
	void recursiveTraverse(Node* current, Node* parent, std::map<std::string, size_t>& namesCount);
	bool recursiveVarDelete(Node* current, Node* parent,std::map<std::string, size_t>& namesCount);
	const std::set<std::string> wrongParents = { "NodeDeclarator", "NodeAssigning" };
	Node* start;
};

