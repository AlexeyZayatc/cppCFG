#include "SymbolTable.h";

void SymbolTable::add(string& name, Symbol* sym)
{
	symbols[name] = sym;
}

Symbol* SymbolTable::get(string& name)
{
	if (symbols.contains(name))
	{
		return symbols[name];
	}
	else
	{
		return nullptr;
	}
}

void SymbolTableStack::push(SymbolTable& table)
{
	tables.push_front(table);
}

void SymbolTableStack::pop()
{
	tables.pop_front();
}

void SymbolTableStack::add(string& name, Symbol* sym)
{
	(*tables.begin()).add(name, sym);
}

Symbol* SymbolTableStack::get(string& name)
{
	for (auto& table : tables)
	{
		Symbol* sym = table.get(name);
		if (sym != nullptr)
			return sym;
	}
	return nullptr;
}