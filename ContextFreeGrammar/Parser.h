#pragma once
#pragma once
#include <vector>;
#include <iterator>;
#include "string"
#include "Token.h";
#include "CLexems.h";
#include "SymbolTable.h"

struct Node {
	Node() = default;
	virtual std::string generate() = 0;
	virtual std::string toStr(int level = 0) = 0;

	char tabSym = ' ';
	string tab = "    ";
	inline string getTab(int level) {
		return string(level * 4, tabSym);
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
};


//тип например int float ...
struct NodeType : public Node
{
	NodeType(string type) : type(type) {}
	virtual std::string generate() override {
		return type;
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + "type" + '\n' + out + tab + type + '\n';
	};

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
	string name;
};

//например: 20, 20.1, 'b', false
struct NodeConstant : Node
{
	NodeConstant(string val, NodeType* type) : val(val), type(type) {}
	virtual std::string generate() override {
		return val;
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + "val" + '\n' + out + tab + val + '\n' + out + typeid(*type).name() + '\n' + type->toStr(level + 1) + '\n';
	};
	string val;
	NodeType* type;
};

//(int) d; (float) b, то есть содержит инфу о касте
struct NodeCast : Node
{
	NodeCast(NodeType* type, Node* expr) : type(type), expr(expr) {}
	virtual std::string generate() override {
		return "(" + type->generate() + ")" + expr->generate();
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*type).name() + '\n' + type->toStr(level + 1) + '\n' + out + typeid(*expr).name() + '\n' + expr->toStr(level + 1);
	};
	NodeType* type;
	Node* expr;
};

//-b, +d, !c, !1 (b,d,c - переменные)
struct NodeUnary : Node
{
	NodeUnary(string unaryOp, NodeCast* expr) : unaryOp(unaryOp), expr(expr) {}
	virtual std::string generate() override {
		return getOperation(unaryOp) + expr->generate();
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + "unaryOp" + '\n' + out + tab + unaryOp + '\n' + out + typeid(*expr).name() + '\n' + expr->toStr(level + 1);
	};
	string unaryOp;
	NodeCast* expr;
};

//содержит операции и их операнды
struct NodeBinaryOperator : Node
{
	NodeBinaryOperator(Node* left, string op, Node* right) : left(left), op(op), right(right) {}
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
};
struct NodeContinue : Node {
	virtual std::string generate() {
		return "continue;\n";
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + "Continue\n";
	};
};

//присваивание значения переменной
struct NodeAssigning : Node
{
	NodeAssigning(NodeVar* var, Node* expr) : var(var), expr(expr) {}
	virtual std::string generate() override {
		return var->generate() + ":=" + expr->generate() + ";\n";
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*var).name() + '\n' + var->toStr(level + 1) + '\n' + out + typeid(*expr).name() + '\n' + expr->toStr(level + 1);
	};
	NodeVar* var;
	Node* expr;
};

//содержит переменную, которую объявляем
struct NodeDeclarator : Node
{
	NodeDeclarator(NodeVar* var) : var(var) {};
	virtual std::string generate() override {
		return var->generate();
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*var).name() + '\n' + var->toStr(level + 1) + '\n';
	};
	NodeVar* var;
};

//содержит переменную, которую объявляем и значение, которое ей присваивается
struct NodeInitDeclarator : NodeDeclarator {
	NodeInitDeclarator(NodeVar* var, Node* assign) : NodeDeclarator(var), assign(assign) {};
	virtual std::string generate() override {
		return "var " + var->generate() + ":=" + assign->generate();
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*var).name() + '\n' + var->toStr(level + 1) + '\n' + out + typeid(*assign).name() + '\n' + assign->toStr(level + 1);
	};
	Node* assign;
};

//объявление переменных
struct NodeDeclaration : Node
{
	NodeDeclaration(NodeType* type, vector<NodeDeclarator*>& declarators) : type(type), declarators(declarators) {};
	virtual std::string generate() override {
		std::string sDeclarators = "";
		for (const auto decl : declarators)
			sDeclarators += decl->generate() + ";\n";
		return sDeclarators;
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		string realOut = out + typeid(*type).name() + '\n' + type->toStr(level + 1) + '\n';
		for (auto& declarator : declarators) {
			realOut += out + typeid(*declarator).name() + '\n' + declarator->toStr(level + 1);
		}
		return realOut;
	};
	NodeType* type;
	vector<NodeDeclarator*> declarators;
};

//лист экспрешенов, например: b, a+3, c%2, (int) a
struct NodeExpressionList : Node
{
	NodeExpressionList(vector<Node*>& expressions) : expressions(expressions) {}
	virtual std::string generate() override {
		std::string result;
		for (const auto expr : expressions) {
			result += expr->generate() + ",";
		}
		return result.substr(0, result.size() - 1);
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		string realOut = "";
		for (auto& expr : expressions) {
			realOut += out + typeid(*expr).name() + '\n' + expr->toStr(level + 1);
		}
		return realOut;
	};
	vector<Node*> expressions;
};

//if
struct NodeIf : Node
{
	NodeIf(NodeExpressionList* condition, Node* ifStatement) : condition(condition), ifStatement(ifStatement) {}
	virtual std::string generate() override {
		return "if " + condition->generate() + " then \n" + ifStatement->generate();
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*condition).name() + '\n' + condition->toStr(level + 1) + '\n' + out + typeid(*ifStatement).name() + '\n' + ifStatement->toStr(level + 1);
	};
	NodeExpressionList* condition;
	Node* ifStatement;
};

//ifelse
struct NodeIfElse : NodeIf
{
	NodeIfElse(NodeExpressionList* condition, Node* ifStatement, Node* elseStatement) : NodeIf(condition, ifStatement), elseStatement(elseStatement) {}
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
	Node* elseStatement;
};

//while
struct NodeWhile : Node
{
	NodeWhile(NodeExpressionList* condition, Node* statement) : condition(condition), statement(statement) {}
	virtual std::string generate() override {
		return "while " + condition->generate() + " do\n" + statement->generate();
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*condition).name() + '\n' + condition->toStr(level + 1) + '\n' + out + typeid(*statement).name() + '\n' + statement->toStr(level + 1);
	};
	NodeExpressionList* condition;
	Node* statement;
};

//printf, scanf
struct NodeReservedFunc : public Node
{
	NodeReservedFunc(NodeExpressionList* parameters) : parameters(parameters) {}
	virtual std::string generate() override {
		return "(" + parameters->generate() + ")";
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*parameters).name() + '\n' + parameters->toStr(level + 1);
	};
	NodeExpressionList* parameters;
};

struct NodePrintF : public NodeReservedFunc
{
	NodePrintF(NodeExpressionList* parameters) : NodeReservedFunc(parameters) {}
	virtual std::string generate() override {
		return "write(" + parameters->generate() + ");\n";
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*parameters).name() + '\n' + parameters->toStr(level + 1);
	};
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
};

//блок { }
class NodeCompoundStatement : public Node
{
public:
	NodeCompoundStatement(vector<Node*>& children) : children(children) {};
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
	vector<Node*> children;
};

//начало программы (аксиома)
class NodeTranslationUnit :public Node
{
public:
	NodeTranslationUnit(NodeCompoundStatement* compStatement) : mainFunc(compStatement) {}
	virtual std::string generate() override {
		return mainFunc->generate();
	}
	virtual std::string toStr(int level) override {
		string out = getTab(level);
		return out + typeid(*mainFunc).name() + '\n' + mainFunc->toStr(level + 1);
	};

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