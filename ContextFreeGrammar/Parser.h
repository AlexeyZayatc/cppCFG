#pragma once
#include <vector>;
#include <iterator>;
#include "string"
#include "Token.h";
#include "CLexems.h";

struct Node {
	Node() = default;
	virtual std::string generate() = 0;

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
class NodeType : public Node
{
public:
	NodeType(string type) : type(type) {}
	virtual std::string generate() override {
		return type;
	}

	string type;
};


//переменная, по сути все ID считаются за переменные в этом парсере
struct NodeVar : public Node
{
	NodeVar(string name) : name(name) {}
	virtual std::string generate() override {
		return name;
	}
	string name;
};

//например: 20, 20.1, 'b', false
struct NodeConstant : Node
{
	NodeConstant(string val, NodeType* type) : val(val), type(type) {}
	virtual std::string generate() override {
		return val;
	}
	string val;
	NodeType* type;
};

//(int) d; (float) b, то есть содержит инфу о касте
struct NodeCast : Node
{
	NodeCast(NodeType* type, Node* expr) : type(type), expr(expr) {}
	virtual std::string generate() override {
		return "("+type->generate()+")" + expr->generate();
	}
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
	string unaryOp;
	NodeCast* expr;
};

//содержит операции и их операнды
struct NodeBinaryOperator : Node
{
	NodeBinaryOperator(Node* left, string op, Node* right) : left(left), op(op), right(right) {}
	virtual std::string generate() override {

		
		std::string sOp = getOperation(op);
		if(sOp =="and" || sOp =="or")
			return "(" + left->generate() + ") " + sOp + " (" + right->generate() + ") ";
		std::string typeLeft = typeid(*left).name();
		std::string typeRight = typeid(*right).name();
		if (typeLeft == "struct NodeExpressionList" && typeRight == "struct NodeExpressionList") {
			return "(" + left->generate() + ") " + sOp + " (" + right->generate() + ") ";
		}
		else if (typeLeft == "struct NodeExpressionList" ) {
			return "(" + left->generate() + ") " + sOp + " " + right->generate() + " ";
		}
		else if (typeRight == "struct NodeExpressionList") {
			return left->generate() + " " + sOp + " (" + right->generate() + ") ";
		}
		return left->generate()+" " + sOp +" " + right->generate();
	}
	Node* left;
	string op;
	Node* right;
};

//break; continue;
struct NodeBreak : Node {
	virtual std::string generate() {
		return "break;\n";
	}
};
struct NodeContinue : Node {
	virtual std::string generate() {
		return "continue;\n";
	}
};

//присваивание значения переменной
struct NodeAssigning : Node 
{
	NodeAssigning(NodeVar* var, Node* expr) : var(var), expr(expr) {}
	virtual std::string generate() override {
		return var->generate() + ":=" + expr->generate()+";\n";
	}
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
	NodeVar* var;
};

//содержит переменную, которую объявляем и значение, которое ей присваивается
struct NodeInitDeclarator : NodeDeclarator {
	NodeInitDeclarator(NodeVar* var, Node* assign) : NodeDeclarator(var), assign(assign) {};
	virtual std::string generate() override {
		return "var "+ var->generate() + ":=" + assign->generate();
	}
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
			result += expr->generate()+",";
		}
		return result.substr(0, result.size() - 1);
	}
	vector<Node*> expressions;
};

//if
struct NodeIf : Node
{
	NodeIf(NodeExpressionList* condition, Node* ifStatement) : condition(condition), ifStatement(ifStatement) {}
	virtual std::string generate() override {
		return "if " + condition->generate() + " then \n" + ifStatement->generate();
	}
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
	Node* elseStatement;
};

//while
struct NodeWhile : Node
{
	NodeWhile(NodeExpressionList* condition, Node* statement) : condition(condition), statement(statement) {}
	virtual std::string generate() override {
		return "while " + condition->generate() + " do\n" + statement->generate();
	}
	NodeExpressionList* condition;
	Node* statement;
};

//printf, scanf
struct NodeReservedFunc : public Node
{
	NodeReservedFunc(NodeExpressionList* parameters) : parameters(parameters) {}
	virtual std::string generate() override {
		return "("+parameters->generate()+")";
	}
	NodeExpressionList* parameters;
};

struct NodePrintF : public NodeReservedFunc
{
	NodePrintF(NodeExpressionList* parameters) : NodeReservedFunc(parameters) {}
	virtual std::string generate() override {
		return "write(" + parameters->generate() + ");\n";
	}
};

struct NodeScanF : NodeReservedFunc
{
	NodeScanF(NodeExpressionList* parameters) : NodeReservedFunc(parameters) {}
	virtual std::string generate() override{
		return "read(" + parameters->generate() + ");\n";
	}
};

//блок { }
class NodeCompoundStatement : public Node
{
public:
	NodeCompoundStatement(vector<Node*>& children) : children(children) {};
	virtual std::string generate() override {
		std::string program = "begin\n";
		for (auto child : children) {
			if(child!=nullptr)
				program += child->generate();
		}
		return program + "end;\n";
	}
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

	NodeCompoundStatement* mainFunc;
};




class Parser
{
public:
	Parser(vector<Token> tokens) : mTokens(tokens), it(mTokens.begin()) {}

	Node* mainFunc()
	{
		require("int", TYPE); it++;
		require(MAIN); it++;
		require(LRBRACKET); it++;
		require(RRBRACKET); it++;
		return new NodeTranslationUnit((NodeCompoundStatement*) compoundStatement());
	}

	Node* castExpression()
	{
		string name = (*it).mLexem;
		string sym = (*it).mLexemType;

		if (sym == "lrbracket") {
			it++;
			if ((*it).mLexemType == "type")
			{
				require(TYPE);
				NodeType* type = new NodeType((*it).mLexemType);
				it++;
				require(RRBRACKET); it++;
				return new NodeCast(type, castExpression());
			}
			NodeExpressionList* expr = (NodeExpressionList*)expressionList();
			require(RRBRACKET); it++;
			return expr;
		}

		if (sym == "plus" || sym == "minus" || sym == "NOT")
		{
			it++;
			return new NodeUnary(sym, (NodeCast*) castExpression());
		}

		if (sym == "id") {
			it++;
			return new NodeVar(name);
		}
		if (sym == "integer" || sym == "float" || sym == "char" || sym == "true" || sym == "false") {
			it++;
			return new NodeConstant(name, new NodeType(sym));
		}

		error();
	}

	Node* multiplicativeExpression()
	{
		Node* left = castExpression();
		string op = (*it).mLexemType;
		while (op == "multiply" || op == "division" || op == "rem")
		{
			it++;
			left = new NodeBinaryOperator(left, op, multiplicativeExpression());
			op = (*it).mLexemType;
		}
		return left;
	}

	Node* additiveExpression()
	{
		Node* left = multiplicativeExpression();
		string op = (*it).mLexemType;
		while (op == "plus" || op == "minus")
		{
			it++;
			left = new NodeBinaryOperator(left, op, additiveExpression());
			op = (*it).mLexemType;
		}
		return left;
	}

	Node* relationalExpression()
	{
		Node* left = additiveExpression();
		string op = (*it).mLexemType;
		while (op == "less" || op == "greater" || op == "lessequal" || op == "greatequal")
		{
			it++;
			left = new NodeBinaryOperator(left, op, relationalExpression());
			op = (*it).mLexemType;
		}
		return left;
	}

	Node* equalityExpression()
	{
		Node* left = relationalExpression();
		string op = (*it).mLexemType;
		while (op == "equal" || op == "notequal")
		{
			it++;
			left = new NodeBinaryOperator(left, op, equalityExpression());
			op = (*it).mLexemType;
		}
		return left;
	}

	Node* logicalAndExpression()
	{
		Node* left = equalityExpression();
		while ((*it).mLexemType == "AND")
		{
			it++;
			left = new NodeBinaryOperator(left, "AND", logicalAndExpression());
		}
		return left;
	}

	Node* logicalOrExpression()
	{
		Node* left = logicalAndExpression();
		while ((*it).mLexemType == "OR")
		{
			it++;
			left = new NodeBinaryOperator(left, "OR", logicalOrExpression());
		}
		return left;
	}

	Node* conditionalExpression()
	{
		return logicalOrExpression();
	}

	Node* expression()
	{
		if ((*it).mLexemType == "id")
		{
			string id = (*it).mLexem;
			it++;
			if ((*it).mLexemType == "set")
			{
				it++;
				return new NodeAssigning(new NodeVar(id), expression());
			}
			it--;
		}

		return conditionalExpression();
	}

	Node* declarator()
	{
		require(IDENTIFIER);
		string id = (*it).mLexem; it++;

		if ((*it).mLexemType == "set")
		{
			it++;
			return new NodeInitDeclarator(new NodeVar(id), expression());
		}
		return new NodeDeclarator(new NodeVar(id));
	}

	Node* declaration()
	{
		require(TYPE);
		NodeType* type = new NodeType((*it).mLexem);
		it++;

		//не через do while, потому что declaration может быть "type_name;"
		vector<NodeDeclarator*> declarators;
		while ((*it).mLexemType != "semicolon" && (*it).mLexemType != "EOF")
		{
			declarators.push_back((NodeDeclarator*) declarator());
			if ((*it).mLexemType != "comma")
				break;
			it++;
		}
		require(SEMICOLON); it++;

		return new NodeDeclaration(type, declarators);
	}

	Node* blockItem()
	{
		//declaration
		if ((*it).mLexemType == "type")
		{
			//std::cout << "Declaration" << endl;
			return declaration();
		}
		//statement
		else
		{
			//std::cout << "Statement" << endl;
			return statement();
		}

	}

	Node* expressionList()
	{
		vector<Node*> expressions;
		do {
			if ((*it).mLexemType == "comma") {
				it++;
			}
			expressions.push_back(expression());
		} while ((*it).mLexemType == "comma" && (*it).mLexemType != "EOF");

		NodeExpressionList* expr = new NodeExpressionList(expressions);
		return expr;
	}

	Node* expressionStatement()
	{

		vector<Node*> expressions;
		while ((*it).mLexemType != "semicolon" && (*it).mLexemType != "EOF")
		{
			expressions.push_back(expression());
			if ((*it).mLexemType != "comma")
				break;
		}
		require(SEMICOLON); it++;
		return new NodeExpressionList(expressions);
	}

	Node* reservedFuncStatement()
	{
		auto lexemType = (*it).mLexemType;
		it++;
		require(LRBRACKET); it++;
		NodeExpressionList* expressions = (NodeExpressionList*)expressionList();
		require(RRBRACKET); it++;
		require(SEMICOLON); it++;
		if (lexemType == "printf")
		{
			return new NodePrintF(expressions);
		}
		if (lexemType == "scanf")
		{
			return new NodeScanF(expressions);
		}
	}

	Node* iterationStatement()
	{
		require(WHILE); it++;
		require(LRBRACKET); it++;
		NodeExpressionList* exprList = (NodeExpressionList*)expressionList();
		require(RRBRACKET); it++;
		Node* body = statement();
		return new NodeWhile(exprList, body);
	}

	Node* selectionStatement()
	{
		require(IF); it++;
		require(LRBRACKET); it++;
		NodeExpressionList* exprList = (NodeExpressionList*) (expressionList());
		require(RRBRACKET); it++;
		Node* body = statement();
		if ((*it).mLexemType == "else")
		{
			it++;
			Node* elseBody = statement();
			return new NodeIfElse(exprList, body, elseBody);
		}
		return new NodeIf(exprList, body);
	}
 
	Node* compoundStatement()
	{
		require(LCBRACKET); it++;
		vector<Node*> blockItems;
		while ((*it).mLexemType != "rcbracket" && (*it).mLexemType != "EOF") {
			blockItems.push_back(blockItem());
		}
		require(RCBRACKET); it++;
		return new NodeCompoundStatement(blockItems);
	}

	Node* statement()
	{
		string tokenType = (*it).mLexemType;
		if (tokenType == "lcbracket")
		{
			return compoundStatement();
		}

		if (tokenType == "id" || tokenType == "integer" || tokenType == "float"
			|| tokenType == "char" || tokenType == "true" || tokenType == "false")
		{
			return expressionStatement();
		}

		if (tokenType == "if")
		{
			return selectionStatement();
		}

		if (tokenType == "while")
		{
			return iterationStatement();
		}

		if (tokenType == "printf" || tokenType == "scanf")
		{
			return reservedFuncStatement();
		}

		if (tokenType == "break")
		{
			it++;
			require(SEMICOLON); it++;
			return new NodeBreak();
		}

		if (tokenType == "continue")
		{
			it++;
			require(SEMICOLON); it++;
			return new NodeContinue();
		}

		error();
	}

	Node* parse()
	{
		if ((*it).mLexemType == "EOF") 
		{
			std::cout << "Empty file" << endl;
		}
		else 
		{
			return mainFunc();
		}
	}

	void require(c_typeOfLexem type) { //просто для красоты на вход кидаю enum
		if ((*it).mLexemType != C_PRESENTATION[type])
		{
			cout << "Excepted token: " + C_PRESENTATION[type] << " at " << (*it).row << ":" << (*it).column <<endl;
			exit(1);
		}
	}

	void require(string name, c_typeOfLexem type) {
		if ((*it).mLexemType != C_PRESENTATION[type] || (*it).mLexem != name)
		{
			cout << "Excepted token: " + C_PRESENTATION[type] << " at " << (*it).row << ":" << (*it).column << endl;
			exit(1);
		}
	}

	void error() {
		cout << "Error at " << (*it).row << ":" << (*it).column << endl;
		exit(1);
	}
private:
	vector<Token> mTokens;
	vector<Token>::iterator it;
};