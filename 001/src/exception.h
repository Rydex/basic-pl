#ifndef EXCEPTION
#define EXCEPTION

#include <string>

class Exception {
private:
	std::string message, details;
public:
	Exception(const std::string& message, const std::string& details);

	std::string as_string() const;
};

class IllegalCharException : public Exception {
public:
	IllegalCharException(char ch);
};

#endif