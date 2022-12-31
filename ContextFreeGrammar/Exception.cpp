#include "Exception.h"

Exception::Exception(std::string_view msg) : _msg(msg) {}

const char* Exception::what() const noexcept
{
	return _msg.data();
}
