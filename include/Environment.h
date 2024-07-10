#pragma once

#include <any>
#include <map>
#include <string>

#include "RuntimeError.h"
#include "Token.h"

class Environment : public std::enable_shared_from_this<Environment> {
    friend class Interpreter;

 public:
    Environment();
    Environment(std::shared_ptr<Environment> enclosing);
    void define(const std::string &name, std::any value);
    void assign(const Token &name, std::any value);
    void assignAt(int distance, const Token &name, std::any value);
    std::any get(const Token &name);
    std::any getAt(int distance, const std::string &name);
    std::shared_ptr<Environment> ancestor(int distance);

    std::shared_ptr<Environment> enclosing;

 private:
    void print_values();

    std::map<std::string, std::any> values;
};