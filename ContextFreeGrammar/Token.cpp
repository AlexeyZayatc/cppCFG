#include <iomanip>
#include "Token.h"
Token::Token() : mLexem(""), mLexemType(""), row(0), column(0) {}
Token::Token(const string& lxm, const string& lxmType, const size_t& _row, const size_t& _column) noexcept
	: mLexem(lxm), mLexemType(lxmType), row(_row), column(_column) {}
Token::Token(const Token& tkn) noexcept
	: mLexem(tkn.mLexem), mLexemType(tkn.mLexemType), row(tkn.row), column(tkn.column) {}
Token::Token(Token&& tkn) noexcept :
	mLexem(move(tkn.mLexem)),
	mLexemType(move(tkn.mLexemType)),
	row(move(tkn.row)),
	column(move(tkn.column)) {}
Token& Token::operator=(const Token& other) noexcept {
	if (this == &other)
		return *this;
	mLexem = other.mLexem;
	mLexemType = other.mLexemType;
	row = other.row;
	column = other.column;
	return *this;
}

Token& Token::operator=(Token&& other) noexcept {
	mLexem = move(other.mLexem);
	mLexemType = move(other.mLexemType);
	row = move(other.row);
	column = move(other.column);
	return *this;
}
std::ostream& operator <<(std::ostream& ostr, const Token& token) {
	ostr << " | " << std::left<< std::setw(15)<< token.mLexem
		 << " | "<<std::setw(15) << token.mLexemType
		 << " | "<<std::setw(15) << token.row
		 << " | "<<std::setw(15) << token.column
		 << " | ";
	return ostr;
}