#include "public/Bruc.h"
#include <algorithm>
#include <iostream>

#include <fstream>
#include <sstream>

#include "private/types.h"

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

bool Bruc::error() {
	return seccions.empty();
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
T Bruc::get(std::string s, std::string k) 
{
	if (!exists(s,k)) {
		std::cerr << "[BRUC] - ERROR! No existing key \"" << k <<"\" in section \"" << s <<"\"!"<< std::endl;
		return T{};
	}

	std::string value = seccions.at(s).at(k);

	return parseValue<T>(value);
}

// Contents parsing

// Read file as a string
Bruc Bruc::readFile(std::string file) {
	std::ifstream _file(file);
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

	public:
		// Functions
		Parser(std::string _text) : text(_text), line_start(text.begin()), line_next(text.begin()) { }
		
		// Move iterators to next line, return true if new line exists
		bool getNextLine() {
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
			// Check if s_name already existed
			if (ret.seccions.find(s_name) != ret.seccions.end()) {
				// Print error
				std::cerr << "[BRUC] - ERROR! Section \"" << s_name << "\" already exists!" << std::endl;
				// Return empty
				return Bruc();
			}
			continue;
		}
		// If we are here it's a key value pair
		auto [key, value] = p.lineKeyValue();
		// Check valid key and value
		if (key == "" || value == "") {
			// Print error
			std::cerr << "[BRUC] - ERROR! Invalid key-value pair in section \"" << s_name << "\"!" << std::endl;
			p.printLine();
			// Return empty
			return Bruc();
		}
		// Check if key already exists
		if (s_pairs.find(key) != s_pairs.end()) {
			// Print error
			std::cerr << "[BRUC] - ERROR! Key \"" << key << "\" already exists in section \"" << s_name << "\"!" << std::endl;
			// Return empty
			return Bruc();
		}
		s_pairs[key] = value;
	}
	ret.seccions[s_name] = s_pairs;
	return ret;
}
