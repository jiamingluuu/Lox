#ifndef __JLOX_H
#define __JLOX_H

#include <string>


class Lox {
public:
    static bool hadError;

    static int runFile(const std::string& path);
    static int runPrompt();
    static int run(const std::string &source);

    static void error(int line, const std::string& message);
    static void report(int line, const std::string& where, const std::string& message);
};

#endif /* __JLOX_H */
