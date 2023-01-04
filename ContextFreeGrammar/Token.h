#pragma once
#include <string>
#include <iostream>
using namespace std;
struct Token {
	Token();
	Token(const string& lxm, const string& lxmType, const size_t& _row = 0, const size_t& _column= 0) noexcept;
	Token(const Token& tkn) noexcept;
	Token(Token&& tkn) noexcept;
	Token& operator=(const Token& other) noexcept;
	Token& operator=(Token&& other) noexcept;
	strong_ordering operator<=> (const Token&) const = default;
	friend ostream& operator <<(ostream& ostr, const Token& token);
	string mLexem;
	string mLexemType;
	size_t row;
	size_t column;
};
