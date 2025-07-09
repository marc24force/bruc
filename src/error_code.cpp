#include "Bruc.h"
#include <cstring>
#include <cerrno>

// Constructors
Bruc::error_code::error_code(int code, bool general) : _code(static_cast<Bruc::error_code::error_type>(code)), _general(general), _line(-1), _section(""), _key("") {}
Bruc::error_code::error_code(int code, std::string s, std::string k) : _code(static_cast<Bruc::error_code::error_type>(code)), _general(false), _line(-1), _section(s), _key(k) {}
Bruc::error_code::error_code(int code, int l, std::string s, std::string k) : _code(static_cast<Bruc::error_code::error_type>(code)), _general(false), _line(l), _section(s), _key(k) {}

std::string Bruc::error_code::message() const {
	if (_general) std::strerror(_code);
	switch (_code) {
		case NoError: return "No error";
		case SectionExists: return "Section already exists";
		case KeyExists: return "Duplicated key";
		case EmptyKeyValue: return "Empty key value";
		case EmptyKeyName: return "Empty key name";
		case WrongPairFormat: return "Wrong pair format";
		case WrongTypeFormat: return "Invalid type";
		default: return "Unknown Bruc error";
	}
}

std::string Bruc::error_code::verbose() const {
	if (_general) return std::strerror(_code);
	switch (_code) {
		case NoError: return "No error";
		case SectionExists: return std::string("Line ") + std::to_string(_line) + ": Section \'[" + _section + "]\' already exists";
		case KeyExists: return std::string("Line ") + std::to_string(_line) + ": Duplicated key \'" + _key + "\' in \'[" + _section + "]\'";
		case EmptyKeyValue: return std::string("Line ") + std::to_string(_line) + ": Empty value in key \'" + _key + "\' in \'[" + _section + "]\'";
		case EmptyKeyName: return std::string("Line ") + std::to_string(_line) + ": Empty key name in \'[" + _section + "]\'";
		case WrongPairFormat: return std::string("Line ") + std::to_string(_line) + ": Wrong key format in \'[" + _section + "]\'";
		case WrongTypeFormat: return std::string("Invalid type for \'") + _key + "\' in \'[" + _section + "]\'";
		default: return "Unknown Bruc error";
	}
}

Bruc::error_code::operator bool() const {
	return _code != 0;
}

int Bruc::error_code::value() const {
    return _code;
}

int Bruc::error_code::line() const {
	return _line;
}

std::string Bruc::error_code::section() const {
	return _section;
}

std::string Bruc::error_code::key() const {
	return _key;
}
