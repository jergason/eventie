COMPILER = g++
SHARED_SRC = src/Handler.cpp src/Connection.cpp src/HTTPRequest.cpp src/HTTPResponse.cpp src/Tokenizer.cpp src/epoller.cpp src/Config.cpp
TEST_SRC = $(SHARED_SRC) src/test.cpp
TEST_OBJS = $(patsubst src/%.cpp,obj/%.o,$(TEST_SRC))
SERVER_SRC = $(SHARED_SRC)
SERVER_OBJS = $(patsubst src/%.cpp,obj/%.o,$(SERVER_SRC))
MAIN_FLAGS = -Wall -g
FLAGS = -Wall -c -I inc -g

run: clean bin/web
	mv bin/web ./
	./web
	
debug_run: clean bin/web
	mv bin/web ./
	./web -d

all: clean bin/web

server: bin/web

realclean: clean

clean:
		@ rm -rf bin/*
		@ rm -rf obj/*.o

bin/web: $(SERVER_OBJS)
		@ $(COMPILER) $(MAIN_FLAGS) $(SERVER_OBJS) -o bin/web

obj/%.o: src/%.cpp
		@ $(COMPILER) $(FLAGS) -o $@ $^


