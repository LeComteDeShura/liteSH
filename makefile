all: bin/help bin/litesh

BUILD_SRC := ./build

bin/litesh: $(BUILD_SRC)/main.o $(BUILD_SRC)/client.o $(BUILD_SRC)/server.o $(BUILD_SRC)/func.o
	gcc  $(BUILD_SRC)/main.o $(BUILD_SRC)/client.o $(BUILD_SRC)/server.o $(BUILD_SRC)/func.o -ldl  -o litesh

$(BUILD_SRC)/main.o: src/main.c
	gcc -c src/main.c   -o $(BUILD_SRC)/main.o

$(BUILD_SRC)/client.o: src/client.c
	gcc  -c src/client.c -o $(BUILD_SRC)/client.o

$(BUILD_SRC)/server.o: src/server.c
	gcc  -c src/server.c -o $(BUILD_SRC)/server.o

$(BUILD_SRC)/func.o: src/func.c
	gcc  -c src/func.c -o $(BUILD_SRC)/func.o

bin/help: $(BUILD_SRC)/help.o
	gcc -shared $(BUILD_SRC)/help.o -o lib/help.so

$(BUILD_SRC)/help.o: lib/help.c
	gcc  -c lib/help.c -o $(BUILD_SRC)/help.o
