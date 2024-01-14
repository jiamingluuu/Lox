#ifndef __ENVIRONMENT_H
#define __ENVIRONMENT_H

#include <any>
#include <string>
#include <map>

#include "Token.h"
#include "RuntimeError.h"

class Environment : public std::enable_shared_from_this<Environment> {
    friend class Interpreter;
public:
    std::shared_ptr<Environment> enclosing;

    Environment();
    Environment(std::shared_ptr<Environment> enclosing);

    void define(const std::string &name, std::any value);
    void assign(const Token &name, std::any value);
    void assignAt(int distance, const Token &name, std::any value);
    std::any get(const Token &name);
    std::any getAt(int distance, const std::string &name);
    
    std::shared_ptr<Environment> ancestor(int distance);

private:
    std::map<std::string, std::any> values;
    void print_values();
};

#endif /* __ENVIRONMENT_H */
