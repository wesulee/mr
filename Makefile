CC=g++
CFLAGS=-c -std=c++11 `sdl2-config --cflags` -pedantic -Wall -Wextra
LDFLAGS=`sdl2-config --libs` -lSDL2_ttf -lboost_system -lboost_filesystem -lboost_program_options -lboost_serialization
DEBUG=-g -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wundef
SRC_DIR=src
BUILD_DIR=build
SOURCES=$(wildcard $(SRC_DIR)/*.cpp)
OBJECTS=$(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
EXE=$(BUILD_DIR)/mr


all: $(SOURCES) $(EXE)

debug: CFLAGS += $(DEBUG)
debug: all

release: CFLAGS += -O2 -DNDEBUG
release: all

$(EXE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXE) $(LDFLAGS)

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJECTS)
