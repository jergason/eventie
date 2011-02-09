COMPILER = g++
SHARED_SRC = src/epoller.cpp src/Handler.cpp src/Connection.cpp
TEST_SRC = $(SHARED_SRC) src/test.cpp
TEST_OBJS = $(patsubst src/%.cpp,obj/%.o,$(TEST_SRC))
SERVER_SRC = $(SHARED_SRC)
SERVER_OBJS = $(patsubst src/%.cpp,obj/%.o,$(SERVER_SRC))
MAIN_FLAGS = -Wall -g -lpthread
FLAGS = -Wall -c -I inc -g

all: clean bin/server

server: bin/server

test: bin/test

realclean: clean

clean: 
		@ rm -rf bin/*
		@ rm -rf obj/*.o

bin/server: $(SERVER_OBJS)
		@ $(COMPILER) $(MAIN_FLAGS) $(SERVER_OBJS) -o bin/server

bin/test: $(TEST_OBJS)
		@ $(COMPILER) $(TEST_OBJS) -o bin/test

obj/%.o: src/%.cpp
		@ $(COMPILER) $(FLAGS) -o $@ $^


