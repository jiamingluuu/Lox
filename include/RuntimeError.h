#ifndef __RUNTIMEERROR_H
#define __RUNTIMEERROR_H

#include <string>

#include "Token.h"

class RuntimeError : public std::runtime_error {
public:
    Token token;
    RuntimeError(Token token, std::string message);
};

#endif /* __RUNTIMEERROR_H */
