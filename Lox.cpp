#include <iostream>
#include <fstream>

#include "Lox.h"
#include "Scanner.h"
#include "Token.h"
#include "TokenType.h"

int Lox::runFile(const std::string& path) {
    std::ifstream file(path);
    std::string line;
    std::string source;

    while (std::getline(std::cin, line)) {
        source += line + "\n";        
    }    
    
    run(source);

    if (Lox::hadError) {
        exit(1);
    }
}

int Lox::runPrompt() {
    while (true) {
        std::cout << "> ";
        std::string line;
        std::getline(std::cin, line);
        if (line.length() == 0) { break; }
        run(line);
        Lox::hadError = false;
    }
}

int Lox::run(const std::string &source) {
    Scanner scanner{source};
    std::vector<Token> tokens = scanner.scanTokens();

    for (Token token : tokens) {
        std::cout << token << "\n";
    }
}

void error(int line, const std::string& message) {
    Lox::report(line, "", message);
}

void Lox::report(int line, const std::string& where, const std::string& message) {
    std::cout << "[line " << line << "] Error" << where << ": " << message << "\n";
    Lox::hadError = true;
}
