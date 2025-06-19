#ifndef EXCEPTION
#define EXCEPTION

#include <optional>
#include <string>
#include "context.h"
#include "position.h"

class Exception {
protected:
  Position pos_start, pos_end;
  std::string message, details;
  
public:
  Exception(
    const Position& pos_start,
    const Position& pos_end,
    const std::string& message,
    const std::string& details
  );

  virtual std::string as_string() const;
};

class IllegalCharException : public Exception {
public:
  IllegalCharException(
    const Position& pos_start,
    const Position& pos_end,
    char ch
  );
};

class InvalidSyntaxException : public Exception {
public:
  InvalidSyntaxException(
    const Position& pos_start,
    const Position& pos_end,
    const std::string& details = ""
  );
};

class RTException : public Exception {
protected:
  std::optional<Context> context;

public:
  RTException(
    const std::optional<Context>& context,
    const Position& pos_start,
    const Position& pos_end,
    const std::string& details = ""
  );

  std::string as_string() const override;
  std::string generate_traceback() const;
};

class ExpectedCharException : public Exception {
public:
  ExpectedCharException(
    const Position& pos_start,
    const Position& pos_end,
    const std::string& details
  );
};

std::string string_with_arrows(
  const std::string& text,
  const Position& pos_start,
  const Position& pos_end
);

#endif