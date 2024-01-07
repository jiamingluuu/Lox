#include "../include/Environment.h"
#include "../include/Interpreter.h"
#include "../include/LoxFunction.h"
#include "../include/LoxReturn.h"
#include "../include/Stmt.h"

LoxFunction::LoxFunction(
        std::shared_ptr<FunctionStmt> declaration,
        std::shared_ptr<Environment> closure) 
    : declaration(std::move(declaration)), closure(std::move(closure)) {}

int LoxFunction::arity() {
    return declaration->parameters.size();
}

std::any LoxFunction::call(
        Interpreter& interpreter, 
        std::vector<std::any> arguments) {
    std::shared_ptr<Environment> environment = 
        std::make_shared<Environment>(closure);
    for (int i = 0; i < declaration->parameters.size(); i++) {
        environment->define(declaration->parameters[i].lexeme,
                            arguments[i]);
    }

    try {
        interpreter.executeBlock(declaration->body, environment);
    } catch(LoxReturn returnValue) {
        return returnValue.value;
    }

    return {};
}

std::string LoxFunction::toString() {
    return "<fn " + declaration->name.lexeme + ">";
}
