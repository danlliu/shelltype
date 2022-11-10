
CXX=g++
CXXFLAGS=-std=c++17 -O3 -Wall -Werror

SRC=src/
BUILD=build/

CPP=$(wildcard $(SRC)*.cpp)
HPP=$(wildcard $(SRC)*.hpp)
OBJ=$(CPP:$(SRC)%.cpp=$(BUILD)%.o)
EXE=shelltype

.SUFFIXES: 

$(BUILD)%.o: $(SRC)%.cpp
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) -lform -lncurses $^ -o $@


clean:
	rm -rf build
	rm -f $(EXE)

.PHONY: clean
