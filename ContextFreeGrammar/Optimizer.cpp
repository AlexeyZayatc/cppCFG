#include "Optimizer.h"

Optimizer::Optimizer(Node* _start) : start(_start) {}

void Optimizer::optimizeCode() {
	deleteUnusedVars2();
}

void Optimizer::deleteUnusedVars1() {
	std::map<std::string, size_t> namesCount;
	std::map<std::string, size_t> oldNamesCount;
	do {
		recursiveTraverse(start, nullptr, namesCount);
		oldNamesCount = namesCount;

		recursiveVarDelete(start, nullptr, namesCount);
	} while (oldNamesCount == namesCount);

}

void Optimizer::recursiveTraverse(Node* current, Node* parent, std::map<std::string, size_t>& namesCount)
{

	for (auto& child : current->children)
		recursiveTraverse(child, current, namesCount);

	if (current->getNodeType() == "NodeVar" && !wrongParents.contains(parent->getNodeType())) {

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
			if (current->getNodeType() == "NodeCompoundStatement") {
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

void Optimizer::deleteUnusedVars2() {
	std::map<std::string, size_t> namesCount;
	bool found;
	do {
		found = false;
		namesCount.clear();
		deleteUnusedVarsRecursive(start, namesCount, found);
	} while (found);
}

void Optimizer::deleteUnusedVarsRecursive(Node* current, std::map<std::string, size_t>& namesCount, bool& found) {
	for (auto& child : current->children) {
		std::string nodeType = child->getNodeType();
		if (nodeType == "NodeDeclaration") {
			NodeDeclaration* declaration = static_cast<NodeDeclaration*>(child);
			for (auto it = declaration->children.begin() + 1; it != declaration->children.end(); ++it) {
				NodeDeclarator* decl = static_cast<NodeDeclarator*>(*it);
				std::string varName = decl->var->name;
				namesCount[varName] = 0;
				NodeInitDeclarator* initDecl = dynamic_cast<NodeInitDeclarator*>(decl);
				if (initDecl != NULL) {
					if (initDecl->assign->getNodeType() == "NodeVar") {
						std::string varName = static_cast<NodeVar*>(initDecl->assign)->name;
						++namesCount[varName];
					}
					else
						deleteUnusedVarsRecursive(initDecl->assign, namesCount, found);
				}
			}
		}
		else if (nodeType == "NodeVar") {
			std::string varName = static_cast<NodeVar*>(child)->name;
			++namesCount[varName];
		}
		else if (nodeType != "NodeAssigning") {
			deleteUnusedVarsRecursive(child, namesCount, found);
		}
	}

	if (current->getNodeType() != "NodeCompoundStatement") {
		return;
	}

	//удал€ем все пройденные объ€влени€ переменных и присваивани€
	for (auto& child : current->children) {
		std::string nodeType = child->getNodeType();
		if (nodeType == "NodeDeclaration") {
			child->children.erase(
				std::remove_if(
					child->children.begin(),
					child->children.end(),
					[&namesCount, &found](Node* node) {
						std::string nodeType = node->getNodeType();
						if (nodeType == "NodeDeclarator" || nodeType == "NodeInitDeclarator") {
							std::string varName = static_cast<NodeDeclarator*>(node)->var->name;
							if (namesCount[varName] == 0) {
								found = true;
								return true;
							}
						}
						return false;
					}
				),
				child->children.end()
			);
		}
		else if (nodeType == "NodeExpressionList") {
			std::vector<Node*>& exprs = child->children;
			exprs.erase(
				std::remove_if(exprs.begin(), exprs.end(),
					[&namesCount](Node* node) {
						if (node->getNodeType() == "NodeAssigning") {
							std::string varName = static_cast<NodeAssigning*>(node)->var->name;
							if (namesCount[varName] == 0) {
								return true;
							}
						}
						return false;
					}
				),
				exprs.end()
			);
			//TODO: если экспрешнлист пуст, то удал€ем его
		}
	}

	//удал€ем все пройденные объ€влени€ переменных из мапы
	for (auto& child : current->children) {
		std::string nodeType = child->getNodeType();
		if (nodeType == "NodeDeclaration") {
			for (auto& lowChild : child->children) {
				std::string nodeType = lowChild->getNodeType();
				if (nodeType == "NodeDeclarator" || nodeType == "NodeInitDeclarator") {
					std::string varName = static_cast<NodeDeclarator*>(lowChild)->var->name;
					namesCount.erase(varName);
				}
			}
		}
	}

}

