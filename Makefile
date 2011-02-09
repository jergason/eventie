COMPILER = g++
SHARED_SRC = src/Handler.cpp src/Connection.cpp src/epoller.cpp
TEST_SRC = $(SHARED_SRC) src/test.cpp
TEST_OBJS = $(patsubst src/%.cpp,obj/%.o,$(TEST_SRC))
SERVER_SRC = $(SHARED_SRC)
SERVER_OBJS = $(patsubst src/%.cpp,obj/%.o,$(SERVER_SRC))
MAIN_FLAGS = -Wall -g
FLAGS = -Wall -c -I inc -g

all: clean bin/server

server: bin/server

realclean: clean

clean:
		@ rm -rf bin/*
		@ rm -rf obj/*.o

bin/server: $(SERVER_OBJS)
		@ $(COMPILER) $(MAIN_FLAGS) $(SERVER_OBJS) -o bin/server

obj/%.o: src/%.cpp
		@ $(COMPILER) $(FLAGS) -o $@ $^


