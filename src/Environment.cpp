#include <iostream>

#include "../include/Environment.h"
#include "../include/RuntimeError.h"

Environment::Environment() : enclosing{nullptr} {}

Environment::Environment(std::shared_ptr<Environment> enclosing) 
    : enclosing(std::move(enclosing)) {}

void Environment::define(const std::string &name, std::any value) {
    values[name] = std::move(value);
}

void Environment::print_values() {
    for (auto [key, value] : values) {
        std::cout << "[" << key << "] ";
    }
    std::cout << "\n";
}

void Environment::assign(const Token &name, std::any value) {
    auto element = values.find(name.lexeme);
    if (element != values.end()) {
        element->second = std::move(value);
        return;
    }

    if (enclosing != nullptr) {
        enclosing->assign(name, std::move(value));
        return;
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

std::any Environment::get(const Token &name) {
    auto element = values.find(name.lexeme);
    if(element != values.end()) {
        return element->second;
    }

    if (enclosing != nullptr) return enclosing->get(name);

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

std::shared_ptr<Environment> Environment::ancestor(int distance) {
    std::shared_ptr<Environment> environment 
        = std::make_shared<Environment>(*this);
    
    for (int i = 0; i < distance; ++i) {
        environment = environment->enclosing;
    }

    return environment;
}

std::any Environment::getAt(int distance, const std::string &name) {
    return ancestor(distance)->values[name];
}

void Environment::assignAt(int distance, const Token &name, std::any value) {
    ancestor(distance)->values[name.lexeme] = std::move(value);
}