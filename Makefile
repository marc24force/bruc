
all: lib/libbruc.a

build/Bruc.o: src/Bruc.cpp include/private/types.h
	mkdir build -p
	g++ -c -Os $< -Iinclude -o $@

lib/libbruc.a: build/Bruc.o
	mkdir lib -p
	ar rcs $@ $<

.PHONY: test
test: test.exe
	@./test.exe

test.exe: example/main.cpp lib/libbruc.a
	g++ $< -Iinclude/public -Llib/ -lbruc -Wl,--gc-sections -o test.exe

.PHONY: clean
clean: 
	rm -rf build lib test.exe

