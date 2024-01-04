#ifndef __ENVIRONMENT_H
#define __ENVIRONMENT_H

#include <any>
#include <string>
#include <map>

#include "Token.h"

class Environment {
public:
    std::shared_ptr<Environment> enclosing;

    Environment();
    Environment(std::shared_ptr<Environment> enclosing);

    void define(std::string name, std::any value);
    void assign(Token name, std::any value);
    std::any get(Token name);

private:
    std::map<std::string, std::any> values;
    void print_values();
};

#endif /* __ENVIRONMENT_H */
