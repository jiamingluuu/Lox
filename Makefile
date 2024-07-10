CXX = g++
C_FLAGS = -g -Wall -Werror -std=c++17
RM = rm

OBJ_PATH = build/
SRC_PATH = src/

OBJ1 = Lox.o \
	   Scanner.o \
	   Token.o \
	   Parser.o \
	   Interpreter.o \
	   RuntimeError.o \
	   Environment.o \
	   LoxFunction.o \
	   Resolver.o

OBJ = $(patsubst %,$(OBJ_PATH)%,$(OBJ1))

TARGET = clox

$(TARGET): $(OBJ)
	@echo [INFO] Creating Binary Executable [$(TARGET)]
	@$(CXX) -o $@ $^

$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp
	@echo [CC] $<
	@$(CXX) $(C_FLAGS) -o $@ -c $<

.PHONY: clean
clean: 
	$(RM) -rfv $(OBJ_PATH)*
	$(RM) $(TARGET)
