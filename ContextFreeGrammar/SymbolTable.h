#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <deque>
using namespace std;

struct Symbol
{
	Symbol(string name) : name(name) {}
	string name;
};

typedef unordered_map<string, Symbol*> symbolDict;

class SymbolTable
{
public:
	void add(string& name, Symbol* sym);
	Symbol* get(string& name);

private:
	symbolDict symbols;
};

class SymbolTableStack
{
public:
	void push(SymbolTable& table);
	void pop();
	void add(string& name, Symbol* sym);
	Symbol* get(string& name);

	inline void error(string msg) {
		cout << "Semantic analysis error: " << msg << endl;
		exit(1);
	}
private:
	deque<SymbolTable> tables;
};

struct SymbolVar : Symbol
{
	SymbolVar(string name) : Symbol(name) {}
};