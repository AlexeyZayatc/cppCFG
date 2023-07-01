#pragma once
#include "Parser.h"



Parser::Parser(vector<Token> tokens) : mTokens(tokens), itToken(mTokens.begin()) {}

Node* Parser::mainFunc()
{
	require("int", TYPE); itToken++;
	require(MAIN); itToken++;
	require(LRBRACKET); itToken++;
	require(RRBRACKET); itToken++;
	return new NodeTranslationUnit((NodeCompoundStatement*) compoundStatement());
}

//здесь не только каст, но и константы и переменные
Node* Parser::castExpression()
{
	string name = (*itToken).mLexem;
	string sym = (*itToken).mLexemType;

	if (sym == "lrbracket") {
		itToken++;
		if ((*itToken).mLexemType == "type")
		{
			require(TYPE);
			NodeType* type = new NodeType((*itToken).mLexemType);
			itToken++;
			require(RRBRACKET); itToken++;
			return new NodeCast(type, castExpression());
		}
		NodeExpressionList* expr = (NodeExpressionList*)expressionList();
		require(RRBRACKET); itToken++;
		return expr;
	}

	if (sym == "plus" || sym == "minus" || sym == "NOT")
	{
		itToken++;
		return new NodeUnary(sym, (NodeCast*) castExpression());
	}

	if (sym == "id") {
		if (symStack.get(name) == nullptr)
		{
			semanticError(name, false);
		};
		itToken++;
		return new NodeVar(name);
	}
	if (sym == "integer" || sym == "float" || sym == "char" || sym == "true" || sym == "false") {
		itToken++;
		return new NodeConstant(name, new NodeType(sym));
	}

	error("Unexcpected symbol");
}

Node* Parser::multiplicativeExpression()
{
	Node* left = castExpression();
	string op = (*itToken).mLexemType;
	while (op == "multiply" || op == "division" || op == "rem")
	{
		itToken++;
		left = new NodeBinaryOperator(left, op, multiplicativeExpression());
		op = (*itToken).mLexemType;
	}
	return left;
}

Node* Parser::additiveExpression()
{
	Node* left = multiplicativeExpression();
	string op = (*itToken).mLexemType;
	while (op == "plus" || op == "minus")
	{
		itToken++;
		left = new NodeBinaryOperator(left, op, additiveExpression());
		op = (*itToken).mLexemType;
	}
	return left;
}

Node* Parser::relationalExpression()
{
	Node* left = additiveExpression();
	string op = (*itToken).mLexemType;
	while (op == "less" || op == "greater" || op == "lessequal" || op == "greatequal")
	{
		itToken++;
		left = new NodeBinaryOperator(left, op, relationalExpression());
		op = (*itToken).mLexemType;
	}
	return left;
}

Node* Parser::equalityExpression()
{
	Node* left = relationalExpression();
	string op = (*itToken).mLexemType;
	while (op == "equal" || op == "notequal")
	{
		itToken++;
		left = new NodeBinaryOperator(left, op, equalityExpression());
		op = (*itToken).mLexemType;
	}
	return left;
}

Node* Parser::logicalAndExpression()
{
	Node* left = equalityExpression();
	while ((*itToken).mLexemType == "AND")
	{
		itToken++;
		left = new NodeBinaryOperator(left, "AND", logicalAndExpression());
	}
	return left;
}

Node* Parser::logicalOrExpression()
{
	Node* left = logicalAndExpression();
	while ((*itToken).mLexemType == "OR")
	{
		itToken++;
		left = new NodeBinaryOperator(left, "OR", logicalOrExpression());
	}
	return left;
}

Node* Parser::conditionalExpression()
{
	return logicalOrExpression();
}

Node* Parser::expression()
{
	if ((*itToken).mLexemType == "id")
	{
		string id = (*itToken).mLexem;
		itToken++;
		if ((*itToken).mLexemType == "set")
		{
			if (symStack.get(id) == nullptr)
			{
				semanticError(id, false);
			};
			itToken++;
			return new NodeAssigning(new NodeVar(id), expression());
		}
		itToken--;
	}

	return conditionalExpression();
}

Node* Parser::declarator()
{
	require(IDENTIFIER);
	string id = (*itToken).mLexem; itToken++;
	NodeDeclarator* node;
	if (symStack.get(id) != nullptr)
	{
		semanticError(id, true);
	};
	if ((*itToken).mLexemType == "set")
	{
		itToken++;
		node = new NodeInitDeclarator(new NodeVar(id), expression());
	}
	else
	{
		node = new NodeDeclarator(new NodeVar(id));
	}
	symStack.add(id, new SymbolVar(id));
	return node;

}

Node* Parser::declaration()
{
	require(TYPE);
	NodeType* type = new NodeType((*itToken).mLexem);
	itToken++;

	vector<NodeDeclarator*> declarators;
	while ((*itToken).mLexemType != "semicolon" && (*itToken).mLexemType != "EOF")
	{
		declarators.push_back((NodeDeclarator*) declarator());
		if ((*itToken).mLexemType != "comma")
			break;
		itToken++;
	}
	require(SEMICOLON); itToken++;

	return new NodeDeclaration(type, declarators);
}

Node* Parser::blockItem()
{
	//declaration
	if ((*itToken).mLexemType == "type")
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

Node* Parser::expressionList()
{
	vector<Node*> expressions;
	do {
		if ((*itToken).mLexemType == "comma") {
			itToken++;
		}
		expressions.push_back(expression());
	} while ((*itToken).mLexemType == "comma" && (*itToken).mLexemType != "EOF");

	NodeExpressionList* expr = new NodeExpressionList(expressions);
	return expr;
}

Node* Parser::expressionStatement()
{

	vector<Node*> expressions;
	while ((*itToken).mLexemType != "semicolon" && (*itToken).mLexemType != "EOF")
	{
		expressions.push_back(expression());
		if ((*itToken).mLexemType != "comma")
			break;
	}
	require(SEMICOLON); itToken++;
	return new NodeExpressionList(expressions);
}

Node* Parser::reservedFuncStatement()
{
	auto lexemType = (*itToken).mLexemType;
	itToken++;
	require(LRBRACKET); itToken++;
	NodeExpressionList* expressions = (NodeExpressionList*)expressionList();
	require(RRBRACKET); itToken++;
	require(SEMICOLON); itToken++;
	if (lexemType == "printf")
	{
		return new NodePrintF(expressions);
	}
	if (lexemType == "scanf")
	{
		return new NodeScanF(expressions);
	}
}

Node* Parser::iterationStatement()
{
	require(WHILE); itToken++;
	require(LRBRACKET); itToken++;
	NodeExpressionList* exprList = (NodeExpressionList*)expressionList();
	require(RRBRACKET); itToken++;
	Node* body = statement();
	return new NodeWhile(exprList, body);
}

Node* Parser::selectionStatement()
{
	require(IF); itToken++;
	require(LRBRACKET); itToken++;
	NodeExpressionList* exprList = (NodeExpressionList*) (expressionList());
	require(RRBRACKET); itToken++;
	Node* body = statement();
	if ((*itToken).mLexemType == "else")
	{
		itToken++;
		Node* elseBody = statement();
		return new NodeIfElse(exprList, body, elseBody);
	}
	return new NodeIf(exprList, body);
}
 
Node* Parser::compoundStatement()
{
	require(LCBRACKET); itToken++;
	SymbolTable table;
	symStack.push(table);
	vector<Node*> blockItems;
	while ((*itToken).mLexemType != "rcbracket" && (*itToken).mLexemType != "EOF") {
		blockItems.push_back(blockItem());
	}
	require(RCBRACKET); itToken++;
	symStack.pop();
	return new NodeCompoundStatement(blockItems);
}

Node* Parser::statement()
{
	string tokenType = (*itToken).mLexemType;
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
		itToken++;
		require(SEMICOLON); itToken++;
		return new NodeBreak();
	}

	if (tokenType == "continue")
	{
		itToken++;
		require(SEMICOLON); itToken++;
		return new NodeContinue();
	}

	error("Unexcpected symbol");
}

Node* Parser::parse()
{
	if ((*itToken).mLexemType == "EOF") 
	{
		std::cout << "Empty file" << endl;
	}
	else 
	{
		return mainFunc();
	}
}

void Parser::require(c_typeOfLexem type) { //просто для красоты на вход кидаю enum
	if ((*itToken).mLexemType != C_PRESENTATION[type])
	{
		error("Excepted token : " + C_PRESENTATION[type]);
	}
}

void Parser::require(string name, c_typeOfLexem type) {
	if ((*itToken).mLexemType != C_PRESENTATION[type] || (*itToken).mLexem != name)
	{
		error("Excepted token : " + C_PRESENTATION[type] + " (" + name + ")");
	}
}

void Parser::error(string msg) {
	cout << "Parser error: " << msg << " (" << (*itToken).row << ":" << (*itToken).column << ")" << endl;
	exit(1);
}

void Parser::semanticError(string varName, bool varDefined) {
	string erMsg = varDefined ? "Variable \'" + varName + "\' is already defined" :
								"Variable \'" + varName + "\' is not defined";
	symStack.error(erMsg + " (" + to_string((*itToken).row) + ":" + to_string((*itToken).column) + ")");
}