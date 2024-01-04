#include <iostream>

#include "../include/Environment.h"
#include "../include/RuntimeError.h"

Environment::Environment() : enclosing{nullptr} {}

Environment::Environment(std::shared_ptr<Environment> enclosing) 
    : enclosing(std::move(enclosing)) {}

void Environment::define(std::string name, std::any value) {
    values[name] = std::move(value);
}

void Environment::print_values() {
    for (auto [key, value] : values) {
        std::cout << "[" << key << "] = " << std::any_cast<double>(value) << "; ";
    }
    std::cout << "\n";
}

void Environment::assign(Token name, std::any value) {
    auto element = values.find(name.lexeme);
    if (element != values.end()) {
        element->second = std::move(value);
        return;
    }

    if (enclosing != nullptr) {
        enclosing->assign(name, value);
        return;
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

std::any Environment::get(Token name) {
    auto element = values.find(name.lexeme);
    if(element != values.end()) {
        return element->second;
    }

    if (enclosing != nullptr) return enclosing->get(name);

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}
