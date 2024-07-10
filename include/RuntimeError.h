#pragma once

#include <string>

#include "Token.h"

class RuntimeError : public std::runtime_error {
 public:
    Token token;
    RuntimeError(Token token, std::string message);
};