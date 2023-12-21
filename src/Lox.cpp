#include <iostream>
#include <fstream>

#include "../include/Lox.h"
#include "../include/Parser.h"
#include "../include/Scanner.h"
#include "../include/Token.h"

bool Lox::hadError = false;
int main(int argc, char *argv[]) {
    
    if (argc > 2) {
        std::cout << "Usage: lox [script]\n";
        exit(64);
    } else if (argc == 2) {
        Lox::runFile(argv[1]);
    } else {
        Lox::runPrompt();
    }

    return 0;
}

void Lox::runFile(const std::string& path) {
    std::ifstream file(path);
    std::string line;
    std::string source;

    while (std::getline(std::cin, line)) {
        source += line + "\n";        
    }    
    
    run(source);

    if (hadError) {
        exit(1);
    }
}

void Lox::runPrompt() {
    while (true) {
        std::cout << "> ";
        std::string line;
        std::getline(std::cin, line);
        if (line.length() == 0) { break; }
        run(line);
        hadError = false;
    }
}

void Lox::run(const std::string &source) {
    Scanner scanner{source};
    std::vector<Token> tokens = scanner.scanTokens();

    Parser parser{tokens};
    std::unique_ptr<Expr> expression = parser.parse();

    if (hadError) { 
        return; 
    }
}

void Lox::error(int line, const std::string& message) {
    report(line, "", message);
}

void Lox::report(int line, const std::string& where, const std::string& message) {
    std::cout << "[line " << line << "] Error" << where << ": " << message << "\n";
    hadError = true;
}

void Lox::error(Token token, const std::string& message) {
    if (token.type == END_OF_FILE) {
        report(token.line, " at end", message);
    } else {
        report(token.line, "at '" + token.lexeme + "'", message);
    }
}
