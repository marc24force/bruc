#include "Bruc.h"

#include <algorithm>
#include <iostream>

#include <fstream>
#include <sstream>

#include <filesystem>

/* Define all the supported types */
#define BRUC_INSTANTIATE_GET(T) template T Bruc::get<T>(std::string, std::string, std::string); \
	                        template std::vector<T> Bruc::get<std::vector<T>>(std::string, std::string, std::string)

BRUC_INSTANTIATE_GET(bool);
BRUC_INSTANTIATE_GET(int);
BRUC_INSTANTIATE_GET(unsigned int);
BRUC_INSTANTIATE_GET(long);
BRUC_INSTANTIATE_GET(unsigned long);
BRUC_INSTANTIATE_GET(float);
BRUC_INSTANTIATE_GET(double);
BRUC_INSTANTIATE_GET(std::string);
BRUC_INSTANTIATE_GET(std::filesystem::path);

template<typename>
struct is_vector : std::false_type {};

template<typename T, typename A>
struct is_vector<std::vector<T, A>> : std::true_type {};

template<typename T>
constexpr bool is_vector_v = is_vector<T>::value;

// Parser function
template <typename T>
T parseValue(std::string value, std::error_code& ec) {
	value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](char c) { return !std::isspace(c); }));
	value.erase(std::find_if(value.rbegin(), value.rend(), [](char c) { return !std::isspace(c); }).base(), value.end());

	T result;

	if constexpr (std::is_same_v<T, std::string>){
		if (!value.empty() && value.front() == '"' && value.back() == '"')
			return std::string(value.begin() + 1, value.end() - 1);
		return value;
	} else if constexpr (is_vector_v<T>) {
		using A = typename T::value_type;
		if (!value.empty() && value.front() == '{' && value.back() == '}') {
			std::istringstream ss(std::string(value.begin()+1, value.end()-1));
			std::string item;
			while (std::getline(ss, item, ',')) 
				result.push_back(parseValue<A>(item, ec));

		} else if (value.size() >= 5 && value.front() == '(' && value.back() == ')') {
			auto comma = value.begin();
			for (; *comma != ',' && comma != value.end(); ++comma);
			unsigned int N = parseValue<unsigned int>(std::string(value.begin()+1, comma), ec);
			A M = parseValue<A>(std::string(comma+1, value.end()-1), ec);
			for (int i = 0; i < N; ++i) result.push_back(M);
		} else ec = make_error_code(BrucError::WrongTypeFormat);
		return result;
	} else {
		std::istringstream converter(value);
		if (value.rfind("0x", 0) == 0 || value.rfind("0X", 0) == 0)
			converter >> std::hex;
		if(!(converter >> result)) ec = make_error_code(BrucError::WrongTypeFormat);
		return result;
	}
}

/* BRUC */

Bruc::Bruc() {}
Bruc::Bruc(std::error_code ec, unsigned int ln) : _error(ec), _error_line(ln) { }

bool Bruc::exists(std::string s) {
	auto it = seccions.find(s);
	return it != seccions.end();
}

bool Bruc::exists(std::string s, std::string k) {
	if (exists(s)) {
		auto seccio = seccions.at(s);
		auto it = seccio.find(k);
		return it != seccio.end();
	}
	return false;
}

std::error_code Bruc::error() {
	return _error;
}

unsigned int Bruc::getErrorLine() {
	return _error_line;
}

std::vector<std::string> Bruc::getSections() {
	std::vector<std::string> list;
	for (const auto& [k, _] : seccions)
		list.push_back(k);
	return list;
}

std::vector<std::string> Bruc::getKeys(std::string s) {
	std::vector<std::string> list;
	if (!exists(s)) return list;

	for (const auto& [k, _] : seccions.at(s))
		list.push_back(k);
	return list;
}


template <typename T>
T Bruc::get(std::string s, std::string k, std::string d) 
{
	_error.clear();

	if (!exists(s,k)) return parseValue<T>(d, _error);

	std::string value = seccions.at(s).at(k);

	return parseValue<T>(value, _error);
}

// Contents parsing

// Read file as a string
Bruc Bruc::readFile(std::string file) {
	std::ifstream _file;
	errno = 0;
	_file.open(file);
	if (!_file) return Bruc(std::error_code(errno, std::generic_category()));
	std::stringstream buffer;
	buffer << _file.rdbuf();
	return readString(buffer.str());
}

// Auxiliar class to process the string
class Parser {
	private:
		// Types
		using Iterador = std::string::const_iterator;
		// Variables
		std::string text;
		Iterador line_start, line_end, line_next;
		unsigned int _line;

	public:
		// Functions
		Parser(std::string _text) : _line(0), text(_text), line_start(text.begin()), line_next(text.begin()) { }
		
		// Move iterators to next line, return true if new line exists
		bool getNextLine() {
			// Increment the line number
			++_line;
			// If previous line was the last, return false
			if (line_next == text.end()) return false;
			// Move line end unless it was at the beginnig
			if (line_next != text.begin()) ++line_next;
			// Move the line start to the new line
			line_start = line_next;
			// While not in the end of line, move the iterator
			while(*line_next != '\n' && line_next != text.end()) line_next++;
			// Remove all comments but spare escaped ones
			bool escape = false;
			for (line_end = line_start; line_end < line_next; ++line_end) {
				if (*line_end == '#' && !escape) break;
				if (escape) escape = false;
				else if (*line_end == '\\') escape = true;
			}
			// If empty go to next line
			if (!std::any_of(line_start, line_end, [](char c) {return !std::isspace(c); }))
				return getNextLine(); 
			// Else return true
			return true;
		}

		// Returns true if the line is a section
		bool lineIsSection() {
			auto it = line_start;
			while (it != line_end && std::isspace(*it)) ++it;
			if (it == line_end || *it != '[') return false;

			auto rit = line_end;
			do { --rit; } while (rit != it && std::isspace(*rit));
			if (*rit != ']') return false;

			return std::distance(it, rit) > 1;
		}

		// Returns the section name if is a section
		std::string lineSectionName() {
			auto it = line_start;
			while (it != line_end && std::isspace(*it)) ++it;
			if (it == line_end || *it != '[') return {};

			++it; // skip '['

			auto rit = line_end;
			do { --rit; } while (rit != it && std::isspace(*rit));
			if (rit == it || *rit != ']') return {};

			// Trim spaces inside [ ]
			while (it != rit && std::isspace(*it)) ++it;
			while (rit != it && std::isspace(*(rit - 1))) --rit;

			return std::string(it, rit);
		}

		std::pair<std::string, std::string> lineKeyValue() {
			auto it_eq = line_start;
			//find equal sign
			while (it_eq != line_end && *it_eq != '=') ++it_eq;;
			if (it_eq == line_end) return {};

			// find key
			auto it = line_start;
			auto rit = it_eq;

			// Trim the spaces around key
			while (it != rit && std::isspace(*it)) ++it;
			while (rit != it && std::isspace(*(rit - 1))) --rit;

			std::string key(it, rit);

			// find value
			it = it_eq + 1;
			rit = line_end;
			
			// Trim the spaces around value
			while (it != rit && std::isspace(*it)) ++it;
			while (rit != it && std::isspace(*(rit - 1))) --rit;

			std::string value(it, rit);

			return {key, value};
		}

		// Get current line number
		unsigned int ln() {
			return _line;
		}

		// Print current line
		void printLine() {
			std::string line(line_start, line_end);
			std::cerr << line << std::endl;
		}
};

Bruc Bruc::readString(std::string data) {
	// Create an empty return
	Bruc ret;
	// And initial values
	std::string s_name = "__global__";
	std::unordered_map<std::string, std::string> s_pairs;
	
	// Start the parser
	Parser p(data);
	while (p.getNextLine()) {
		// Start a new section
		if(p.lineIsSection()) {
			// Add a new entry with previous s_name and s_pairs
			ret.seccions[s_name] = s_pairs;
			// Clear s_pairs
			s_pairs.clear();
			// Update s_name
			s_name = p.lineSectionName();
			// Check if s_name already existed and return error
			if (ret.seccions.find(s_name) != ret.seccions.end())
				return Bruc(make_error_code(BrucError::SectionExists), p.ln());
			continue;
		}
		// If we are here it's a key value pair
		auto [key, value] = p.lineKeyValue();

		if (key.empty() || value.empty()) {
			if (key.empty() && value.empty()) return Bruc(make_error_code(BrucError::WrongPairFormat), p.ln());
			else if (key.empty()) return Bruc(make_error_code(BrucError::EmptyKeyName), p.ln());
			else return Bruc(make_error_code(BrucError::EmptyKeyValue), p.ln());
		}
		// Check if key already exists and return error
		if (s_pairs.find(key) != s_pairs.end()) return Bruc(make_error_code(BrucError::KeyExists), p.ln());
		s_pairs[key] = value;
	}
	ret.seccions[s_name] = s_pairs;
	ret._error.clear();
	return ret;
}
