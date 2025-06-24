# BRUC - BRUC is a Runtime User Config

BRUC is a configuration file parser for C++. 
The design is inspired by INI files, and in some cases it might be compatible. 
However there are some features that are not supported and some that have been added as it was needed for my projects.


## Installation

BRUC is a static library to be included into any other project. It could also be used as a shared library with minor
modifications to the Makefile.

Run `make` to compile generate the library file.


## Usage

In order to use BRUC you will need to create an object of class `Bruc`.

Then use the functions `readFile` and `readString` to parse a file or a string object.

Use `error()` to validate that the file was properly read and then you can use `exists(section)` and `exists(section, key)` to verify that the entries exist. 

You can also use `getSections()` and `getKeys(section)` to get a vector with the sections and keys of a section respectively.

Read a variable of type T by using `get<T>(section, key)` method, which parses the data to the type. An example code would look like this:

```c++
int addInts(std::string s, std::string file) {
    Bruc config = Bruc::readFile(file);
    if (config.error()) return -1;
    if (!config.exists(s) return -1;
    unsigned int i = 0;
    for (auto k : config.getKeys(s)) {
        i += config.get<unsigned int>(s, k);
    }
    return i;
}
```

Run `make test` and [example/](example/) to view a more complete example.

## BRUC file syntax
BRUC uses it's own syntax which is similar to that of INI files.

Empty lines and text after a `#` are ignored.

Sections are defined within `[` `]`.
The section name goes from the first non-space character to the last one, that means that quotes or spaces within words are
valid and part of the section name.

### Simple types
The supported types are:
- String: Quotes are optional, the first and last `"` are ignored, any other `"` is part of the string. Multiline is not supported.
- Integers: It supports `int`, `unsigned int`, `long` and `unsigned long`. Hexadecimal or binary input is valid.
- Floats: `float` and `double` are supported even when using cientific notation.
- Boolean: You can get boolean values if they are 1 or 0 in the configuration file.
Each of the entries can be one of this types:

### Vectors
The use of `std::vector` is accepted for any of the simple types. There exist two ways to define a vector:
- Use `{` and `}` to limit the vector and `,` to separate the elements. All space characters before and after the first and last non-space characters are ignored.
- Use `(` and `)` to declare a repetition list of the form `(N,M)`. This generates a vector of N elements which have value M. As an example `(3, yes) = {yes, yes, yes}`.
