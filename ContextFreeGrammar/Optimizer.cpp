#include "Optimizer.h"

Optimizer::Optimizer(Node* _start) : start(_start) {}

void Optimizer::optimizeCode() {
	std::map<std::string, size_t> namesCount;
	std::map<std::string, size_t> oldNamesCount;
	recursiveTraverse(start,nullptr, namesCount);
	while (oldNamesCount != namesCount) {
		oldNamesCount = namesCount;

		recursiveVarDelete(start, nullptr, namesCount);

		namesCount.clear();

		recursiveTraverse(start, nullptr, namesCount);		
	}
}

void Optimizer::recursiveTraverse(Node* current,Node* parent, std::map<std::string, size_t>& namesCount)
{

	for (auto& child : current->children)
		recursiveTraverse(child,current, namesCount);

	if (current->getNodeType() == "NodeVar" && !wrongParents.contains(parent->getNodeType())){

		auto var = static_cast<NodeVar*>(current);
		if (namesCount.count(var->name) == 0)
			namesCount[var->name] = 1;
		else
			namesCount[var->name]++;
	}
}

bool Optimizer::recursiveVarDelete(Node* current, Node* parent, std::map<std::string, size_t>& namesCount)
{
	for (auto it = current->children.begin(); it != current->children.end(); it++) {
		if (wrongParents.contains(current->getNodeType()) && (*it)->getNodeType() == "NodeVar") {
			auto var = static_cast<NodeVar*>(*it);
			return !namesCount.contains(var->name);
		}
		if (recursiveVarDelete(*it, current, namesCount))
			if(current->getNodeType()=="NodeCompoundStatement") {
				current->deleteChild(*it);
				it = current->children.begin();
			}
			else {
				return true;
			}
		if (current->children.size() == 0) break;
	}

	return false;
}
