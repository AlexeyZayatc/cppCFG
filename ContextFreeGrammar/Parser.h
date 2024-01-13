#pragma once
#include <vector>
#include <iterator>
#include <string>
#include <map>

#include "Token.h"
#include "CLexems.h"
#include "SymbolTable.h"


struct Node {
	Node() = default;
	virtual std::string generate() = 0;
	virtual std::string toStr(int level = 0) = 0;
	virtual std::string getNodeType() = 0;

	char tabSym = ' ';
	string tab = "    ";
	inline string getTab(int level) {
		return string(level * 4, tabSym);
	}

	void deleteChild(Node* child) {
		for(auto it = children.begin(); it!=children.end(); it++)
			if (*it == child) {
				children.erase(it);
				break;
			}
	}

	std::string getOperation(const std::string& operation) {
		if (operation == "plus")
			return "+";
		else if (operation == "minus")
			return "-";
		else if (operation == "multiply")
			return "*";
		else if (operation == "division")
			return "/";
		else if (operation == "rem")
			return "mod";
		else if (operation == "less")
			return "<";
		else if (operation == "greater")
			return ">";
		else if (operation == "lessequal")
			return "<=";
		else if (operation == "greatequal")
			return ">=";
		else if (operation == "equal")
			return "=";
		else if (operation == "notequal")
			return "<>";
		else if (operation == "AND")
			return "and";
		else if (operation == "OR")
			return "or";
		return "";
	};
	std::vector<Node*> children;
};


//тип например int float ...
struct NodeType : public Node
{
	NodeType(string type) : type(type) {}
	std::string getType(const std::string& tp) {
		if (tp == "int")
			return "integer";
		if (tp == "float")
			return "real";
		if (tp == "bool")
			return "boolean";
	}
	virtual std::string generate() override {
		return getType(type);
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + "type" + '\n' + out + tab + type + '\n';
	};
	virtual std::string getNodeType() override { return "NodeType"; }
	string type;
};


//переменная, по сути все ID считаются за переменные в этом парсере
struct NodeVar : public Node
{
	NodeVar(string name) : name(name) {}
	virtual std::string generate() override {
		return name;
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + "name" + '\n' + out + tab + name + '\n';
	};
	virtual std::string getNodeType() override { return "NodeVar"; }
	string name;
};

//например: 20, 20.1, 'b', false
struct NodeConstant : Node
{
	NodeConstant(string val, NodeType* type) : val(val), type(type) {
		children.push_back(type);
	}
	virtual std::string generate() override {
		return val;
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + "val" + '\n' + out + tab + val + '\n' + out + typeid(*type).name() + '\n' + type->toStr(level + 1) + '\n';
	};
	virtual std::string getNodeType() override { return "NodeConstant"; }
	string val;
	NodeType* type;
};

//(int) d; (float) b, то есть содержит инфу о касте
struct NodeCast : Node
{
	NodeCast(NodeType* type, Node* expr) : type(type), expr(expr) {
		children.push_back(type);
		children.push_back(expr);
	}
	virtual std::string generate() override {
		return "(" + type->generate() + ")" + expr->generate();
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*type).name() + '\n' + type->toStr(level + 1) + '\n' + out + typeid(*expr).name() + '\n' + expr->toStr(level + 1);
	};
	virtual std::string getNodeType() override { return "NodeCast"; }
	NodeType* type;
	Node* expr;
};

//-b, +d, !c, !1 (b,d,c - переменные)
struct NodeUnary : Node
{
	NodeUnary(string unaryOp, NodeCast* expr) : unaryOp(unaryOp), expr(expr) {
		children.push_back(expr);
	}
	virtual std::string generate() override {
		return getOperation(unaryOp) + expr->generate();
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + "unaryOp" + '\n' + out + tab + unaryOp + '\n' + out + typeid(*expr).name() + '\n' + expr->toStr(level + 1);
	};
	virtual std::string getNodeType() override { return "NodeUnary"; }
	string unaryOp;
	NodeCast* expr;
};

//содержит операции и их операнды
struct NodeBinaryOperator : Node
{
	NodeBinaryOperator(Node* left, string op, Node* right) : left(left), op(op), right(right) {
		children.push_back(left);
		children.push_back(right);
	}
	virtual std::string generate() override {


		std::string sOp = getOperation(op);
		if (sOp == "and" || sOp == "or")
			return "(" + left->generate() + ") " + sOp + " (" + right->generate() + ") ";
		std::string typeLeft = typeid(*left).name();
		std::string typeRight = typeid(*right).name();
		if (typeLeft == "struct NodeExpressionList" && typeRight == "struct NodeExpressionList") {
			return "(" + left->generate() + ") " + sOp + " (" + right->generate() + ") ";
		}
		else if (typeLeft == "struct NodeExpressionList") {
			return "(" + left->generate() + ") " + sOp + " " + right->generate() + " ";
		}
		else if (typeRight == "struct NodeExpressionList") {
			return left->generate() + " " + sOp + " (" + right->generate() + ") ";
		}
		return left->generate() + " " + sOp + " " + right->generate();
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + "op" + '\n' + out + tab + op + '\n' + out + typeid(*left).name() + '\n' + left->toStr(level + 1) + '\n' + out + typeid(*right).name() + '\n' + right->toStr(level + 1);
	};
	virtual std::string getNodeType() override  { return "NodeBinaryOperator"; }
	Node* left;
	string op;
	Node* right;
};

//break; continue;
struct NodeBreak : Node {
	virtual std::string generate() {
		return "break;\n";
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + "Break\n";
	};
	virtual std::string getNodeType() override { return "NodeBreak"; }
};
struct NodeContinue : Node {
	virtual std::string generate() {
		return "continue;\n";
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + "Continue\n";
	};
	virtual std::string getNodeType() override { return "NodeContinue"; }
};

//присваивание значения переменной
struct NodeAssigning : Node
{
	NodeAssigning(NodeVar* var, Node* expr) : var(var), expr(expr) {
		children.push_back(var);
		children.push_back(expr);
	}
	virtual std::string generate() override {
		if (children.size() == 2)
			return var->generate() + ":=" + expr->generate() + ";\n";
		return "";
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*var).name() + '\n' + var->toStr(level + 1) + '\n' + out + typeid(*expr).name() + '\n' + expr->toStr(level + 1);
	};
	virtual std::string getNodeType() override { return "NodeAssigning"; }
	NodeVar* var;
	Node* expr;
};

//содержит переменную, которую объявляем
struct NodeDeclarator : Node
{
	NodeDeclarator(NodeVar* var) : var(var) {
		children.push_back(var);
	};
	virtual std::string generate() override {
		if (children.size() == 1)
			return var->generate();
		return "";
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*var).name() + '\n' + var->toStr(level + 1) + '\n';
	};
	virtual std::string getNodeType() override { return "NodeDeclarator"; }
	NodeVar* var;
};

//содержит переменную, которую объявляем и значение, которое ей присваивается
struct NodeInitDeclarator : NodeDeclarator {
	NodeInitDeclarator(NodeVar* var, Node* assign) : NodeDeclarator(var), assign(assign) {
		children.push_back(assign);
	};
	virtual std::string generate() override {
		if (children.size() == 2)
			return "var " + var->generate() + ":=" + assign->generate();
		return "";
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*var).name() + '\n' + var->toStr(level + 1) + '\n' + out + typeid(*assign).name() + '\n' + assign->toStr(level + 1);
	};
	virtual std::string getNodeType() override  { return "NodeInitDeclarator"; }
	Node* assign;
};

//объявление переменных
struct NodeDeclaration : Node
{
	NodeDeclaration(NodeType* type, vector<NodeDeclarator*>& declarators) : type(type), declarators(declarators) {
		children.push_back(type);
		for (const auto& decl : declarators)
			children.push_back(decl);
	};
	virtual std::string generate() override {
		if (children.size() > 1) {
			std::string sDeclarators;
			std::vector<std::string> declVec;
			auto sType = children[0]->generate();
			// id1,id2,id3 : type;
			for (const auto decl : children) {
				if (decl->getNodeType() == "NodeType") continue;

				std::string nm = typeid(*decl).name();
				if (nm == "struct NodeInitDeclarator")
					sDeclarators += decl->generate() + "; ";
				else
					declVec.push_back(decl->generate());
			}
			std::string multipleDecl = "var ";
			for (const auto& id : declVec) {
				if (id != declVec.back())
					multipleDecl += id + ", ";
				else
					multipleDecl += id + ": " + sType;
			}
			if (multipleDecl != "var ")
				sDeclarators += multipleDecl + ";\n";
			else
				sDeclarators += "\n";

			return sDeclarators;
		}
		return "";
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		string realOut = out + typeid(*type).name() + '\n' + type->toStr(level + 1) + '\n';
		for (auto& declarator : declarators) {
			realOut += out + typeid(*declarator).name() + '\n' + declarator->toStr(level + 1);
		}
		return realOut;
	};
	virtual std::string getNodeType() override { return "NodeDeclaration"; }
	NodeType* type;
	vector<NodeDeclarator*> declarators;
};

//лист экспрешенов, например: b, a+3, c%2, (int) a
struct NodeExpressionList : Node
{
	NodeExpressionList(vector<Node*>& expressions) : expressions(expressions) {
		for (const auto& expr : expressions)
			children.push_back(expr);
	}
	virtual std::string generate() override {
		if (children.size() > 0) {
			std::string result;
			for (const auto expr : expressions) {
				result += expr->generate() + ",";
			}
			return result.substr(0, result.size() - 1);
		}
		return "";
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		string realOut = "";
		for (auto& expr : expressions) {
			realOut += out + typeid(*expr).name() + '\n' + expr->toStr(level + 1);
		}
		return realOut;
	};
	virtual std::string getNodeType() override { return "NodeExpressionList"; }
	vector<Node*> expressions;
};

//if
struct NodeIf : Node
{
	NodeIf(NodeExpressionList* condition, Node* ifStatement) : condition(condition), ifStatement(ifStatement) {
		children.push_back(condition);
		children.push_back(ifStatement);
	}
	virtual std::string generate() override {
		return "if " + condition->generate() + " then \n" + ifStatement->generate();
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*condition).name() + '\n' + condition->toStr(level + 1) + '\n' + out + typeid(*ifStatement).name() + '\n' + ifStatement->toStr(level + 1);
	};
	virtual std::string getNodeType() override { return "NodeIf"; }
	NodeExpressionList* condition;
	Node* ifStatement;
};

//ifelse
struct NodeIfElse : NodeIf
{
	NodeIfElse(NodeExpressionList* condition, Node* ifStatement, Node* elseStatement) : NodeIf(condition, ifStatement), elseStatement(elseStatement) {
		children.push_back(elseStatement);
	}
	virtual std::string generate() override {
		std::string ifBody = ifStatement->generate();
		ifBody = ifBody.substr(0, ifBody.size() - 5);
		return "if " + condition->generate() + " then \n" + ifBody +
			"end else\n" + elseStatement->generate();
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*condition).name() + '\n' + condition->toStr(level + 1) + '\n' + out + typeid(*ifStatement).name() + '\n' + ifStatement->toStr(level + 1) + '\n' + out + typeid(*elseStatement).name() + '\n' + elseStatement->toStr(level + 1);
	};
	virtual std::string getNodeType() override  { return "NodeIfElse"; }
	Node* elseStatement;
};

//while
struct NodeWhile : Node
{
	NodeWhile(NodeExpressionList* condition, Node* statement) : condition(condition), statement(statement) {
		children.push_back(condition);
		children.push_back(statement);
	}
	virtual std::string generate() override {
		return "while " + condition->generate() + " do\n" + statement->generate();
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*condition).name() + '\n' + condition->toStr(level + 1) + '\n' + out + typeid(*statement).name() + '\n' + statement->toStr(level + 1);
	};
	virtual std::string getNodeType() override  { return "NodeWhile"; }
	NodeExpressionList* condition;
	Node* statement;
};

//printf, scanf
struct NodeReservedFunc : public Node
{
	NodeReservedFunc(NodeExpressionList* parameters) : parameters(parameters) {

		children.push_back(parameters);
	}
	virtual std::string generate() override {
		return "(" + parameters->generate() + ")";
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*parameters).name() + '\n' + parameters->toStr(level + 1);
	};
	virtual std::string getNodeType() override  { return "NodeReservedFunc"; }
	NodeExpressionList* parameters;
};

struct NodePrintF : public NodeReservedFunc
{
	NodePrintF(NodeExpressionList* parameters) : NodeReservedFunc(parameters) {
	
	}
	virtual std::string generate() override {
		return "write(" + parameters->generate() + ");\n";
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*parameters).name() + '\n' + parameters->toStr(level + 1);
	};
	virtual std::string getNodeType() override  { return "NodePrintF"; }
};

struct NodeScanF : NodeReservedFunc
{
	NodeScanF(NodeExpressionList* parameters) : NodeReservedFunc(parameters) {}
	virtual std::string generate() override {
		return "read(" + parameters->generate() + ");\n";
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*parameters).name() + '\n' + parameters->toStr(level + 1);
	};
	virtual std::string getNodeType() override  { return "NodeScanF"; }
};

//блок { }
class NodeCompoundStatement : public Node
{
public:
	NodeCompoundStatement(vector<Node*>& _children)  {
		for (auto& child : _children)
			children.push_back(child);
	};
	virtual std::string generate() override {
		std::string program = "begin\n";
		for (auto child : children) {
			if (child != nullptr)
				program += child->generate();
		}
		return program + "end;\n";
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		string realOut = "";
		for (auto& child : children) {
			realOut += out + typeid(*child).name() + '\n' + child->toStr(level + 1);
		}
		return realOut;
	};
	virtual std::string getNodeType() override { return "NodeCompoundStatement"; }
};

//начало программы (аксиома)
class NodeTranslationUnit :public Node
{
public:
	NodeTranslationUnit(NodeCompoundStatement* compStatement) : mainFunc(compStatement) {
		children.push_back(compStatement);
	}
	virtual std::string generate() override {
		return mainFunc->generate();
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*mainFunc).name() + '\n' + mainFunc->toStr(level + 1);
	};
	virtual std::string getNodeType() override { return "NodeTranslationUnit"; }

	NodeCompoundStatement* mainFunc;
};

class Parser
{
public:
	Parser(vector<Token> tokens);

	Node* mainFunc();

	//здесь не только каст, но и константы и переменные
	Node* castExpression();

	Node* multiplicativeExpression();

	Node* additiveExpression();

	Node* relationalExpression();

	Node* equalityExpression();

	Node* logicalAndExpression();

	Node* logicalOrExpression();

	Node* conditionalExpression();

	Node* expression();

	Node* declarator();

	Node* declaration();

	Node* blockItem();

	Node* expressionList();

	Node* expressionStatement();

	Node* reservedFuncStatement();

	Node* iterationStatement();

	Node* selectionStatement();

	Node* compoundStatement();

	Node* statement();

	Node* parse();

	void require(c_typeOfLexem type);

	void require(string name, c_typeOfLexem type);

	void error(string msg);

	void semanticError(string varName, bool varDefined);
private:
	vector<Token> mTokens;
	vector<Token>::iterator itToken;
	SymbolTableStack symStack;

};