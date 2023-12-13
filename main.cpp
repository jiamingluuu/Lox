#include <iostream>
#include <fstream>
#include <list>

#include "include/Lox.h"

int main(int argc, char *argv[]) {
    int exitCode = 0;
    Lox::hadError = false;
    
    if (argc > 1) {
        std::cout << "Usage: lox [script]\n";
        exit(64);
    } else if (argc == 1) {
        Lox::runFile(argv[1]);
    } else {
        Lox::runPrompt();
    }

    return exitCode;
}