# Windows-specific
RM=del

CC=gcc
CXX=g++
MACROS=-D UNICODE -D _UNICODE
CDEFFLAGS=$(MACROS) -std=c99 -Wall -Wextra -Wpedantic -Wconversion -Wdouble-promotion -Wstrict-prototypes
CXXDEFFLAGS=$(MACROS) -std=c++11 -Wall -Wextra -Wpedantic -Wconversion -Wdouble-promotion
DebFlags=-g -O0 -D _DEBUG
RelFlags=-O3 -Wl,--strip-all,--build-id=none,--gc-sections -D NDEBUG -mwindows
LIB=-municode -lgdi32 -lcomctl32 -lwindowscodecs -lole32

BIN=bin
SRC=src
OBJ=obj
TARGET=BrownNoise


SRCFILES=$(wildcard $(SRC)/*.c)
SRCFILES+=$(wildcard $(SRC)/*.cpp)
RSCFILES=$(wildcard $(SRC)/*.rc)

RELOBJFILES=$(SRCFILES:%.cpp=%.cpp.o)
RELOBJFILES:=$(RELOBJFILES:%.c=%.c.o)
RELOBJFILES+=$(RSCFILES:%.rc=%.rc.o)
RELOBJFILES:=$(RELOBJFILES:$(SRC)/%=$(OBJ)/%)

DEBOBJFILES=$(SRCFILES:%.c=%.c.d.o)
DEBOBJFILES:=$(DEBOBJFILES:%.cpp=%.cpp.d.o)
DEBOBJFILES+=$(RSCFILES:%.rc=%.rc.d.o)
DEBOBJFILES:=$(DEBOBJFILES:$(SRC)/%=$(OBJ)/%)


default: debug
.PHONY: rel deb release debug release_files debug_files clean

rel: release
deb: debug

release: $(BIN) release_files
debug:   $(BIN) debug_files

release_files: $(RELOBJFILES)
	$(CXX) $^ -o $(BIN)/$(TARGET).exe $(CXXDEFFLAGS) $(RelFlags) $(LIB)
debug_files: $(DEBOBJFILES)
	$(CXX) $^ -o $(BIN)/deb$(TARGET).exe $(CXXDEFFLAGS) $(DebFlags) $(LIB)


$(OBJ)/%.rc.o: $(SRC)/%.rc $(OBJ)
	windres -i $< -o $@ $(MACROS) -D FILE_NAME='\"$(TARGET).exe\"'
$(OBJ)/%.rc.d.o: $(SRC)/%.rc $(OBJ)
	windres -i $< -o $@ $(MACROS) -D FILE_NAME='\"deb$(TARGET).exe\"'

$(OBJ)/%.c.o: $(SRC)/%.c $(OBJ)
	$(CC) -c $< -o $@ $(CDEFFLAGS) $(RelFlags)
$(OBJ)/%.c.d.o: $(SRC)/%.c $(OBJ)
	$(CC) -c $< -o $@ $(CDEFFLAGS) $(DebFlags)

$(OBJ)/%.cpp.o: $(SRC)/%.cpp $(OBJ)
	$(CXX) -c $< -o $@ $(CXXDEFFLAGS) $(RelFlags)
$(OBJ)/%.cpp.d.o: $(SRC)/%.cpp $(OBJ)
	$(CXX) -c $< -o $@ $(CXXDEFFLAGS) $(DebFlags)

$(OBJ):
	mkdir $(OBJ)

$(BIN):
	mkdir $(BIN)

clean:
	$(RM) /F $(OBJ) /Q
	$(RM) .\\$(BIN)\\$(TARGET).exe
	$(RM) .\\$(BIN)\\deb$(TARGET).exe
