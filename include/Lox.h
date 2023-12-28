#ifndef __LOX_H
#define __LOX_H

#include <string>

#include "Interpreter.h"
#include "RuntimeError.h"
#include "Token.h"

class Lox {
public:
    static bool hadError;
    static bool hadRuntimeError;

    static void runFile(const std::string& path);
    static void runPrompt();
    static void run(const std::string &source);

    static void error(int line, const std::string& message);
    static void report(int line, const std::string& where, const std::string& message);
    static void error(Token token, const std::string& message);
    static void runtimeError(RuntimeError error);
};

#endif /* __LOX_H */
