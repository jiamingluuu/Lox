C_FLAGS = -g -Wall -Werror -std=c++17
CXX = g++
RM = rm

OBJ_PATH = obj/
SRC_PATH = src/

OBJ1 = Lox.o Scanner.o Token.o
OBJ = $(patsubst %,$(OBJ_PATH)%,$(OBJ1))

TARGET = main

$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp
	@echo [CC] $<
	@$(CXX) $(C_FLAGS) -o $@ -c $<

$(TARGET): $(OBJ)
	@echo [INFO] Creating Binary Executable [$(TARGET)]
	@$(CXX) -o $@ $^

.PHONY: clean
clean: 
	${RM} -rfv ${OBJ_PATH}*
	${RM} main