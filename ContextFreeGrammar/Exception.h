#pragma once
#include <exception>
#include <iostream>
class Exception :
    public std::exception
{
public:
    Exception(std::string_view msg);
    virtual const char* what() const noexcept override;
private:
    std::string_view _msg;
};

