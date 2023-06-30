#pragma once
#include "Parser.h"
#include <string>
#include <fstream>

class Generator
{
public:
	Generator(Node* _start) {
		start = _start;
	}
	void generate(std::ofstream& _fileStream) {
		auto program = start->generate();
		program[program.size() - 2] = '.';
		_fileStream << program;
	}
	std::string getResult() {
		auto program = start->generate();
		program[program.size() - 2] = '.';
		return program;
	}
private:
	Node* start;
};

