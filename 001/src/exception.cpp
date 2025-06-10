#include "exception.h"

Exception::Exception(const std::string& message, const std::string& details)
	: message(message), details(details) {}

std::string Exception::as_string() const {
	return message + ": " + details;
}

IllegalCharException::IllegalCharException(char ch)
	: Exception("Illegal Character", "'" + std::string(1, ch) + "'") {}