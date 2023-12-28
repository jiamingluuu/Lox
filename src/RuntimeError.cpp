#include "../include/RuntimeError.h"

RuntimeError::RuntimeError(Token token, std::string message)
    : std::runtime_error(message), token(token) {}
