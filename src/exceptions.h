#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H
#include <iostream>
#include <exception>
class Exception: public std::exception
{
public:
    Exception(const std::string msg): _msg(msg) {}
    ~Exception() throw() {}
    virtual const char *what() const throw()
	{
	    return _msg.c_str();
	}
private:
    std::string _msg;
};
#endif
