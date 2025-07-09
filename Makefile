
all: lib/libbruc.a

build/%.o: src/%.cpp
	mkdir build -p
	g++ -c -Os $< -Iinclude -o $@

lib/libbruc.a: build/error_code.o build/value_access.o build/parser.o build/utils.o
	mkdir lib -p
	ar rcs $@ $^

.PHONY: test
test: test.exe
	@./test.exe

test.exe: example/main.cpp lib/libbruc.a
	g++ $< -Iinclude -Llib/ -lbruc -Wl,--gc-sections -o test.exe

.PHONY: clean
clean: 
	rm -rf build lib test.exe

