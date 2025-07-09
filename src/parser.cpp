#include "Bruc.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// Constructors

// With path
Bruc::Bruc(std::filesystem::path file) {
	error_code ec;
	*this = Bruc(file, ec);
}

Bruc::Bruc(std::filesystem::path file, Bruc::error_code& ec) {
	std::ifstream _file (file);
	if (!_file) ec = error_code(errno, true);
	else {
		std::stringstream buffer;
		buffer << _file.rdbuf();
		parse(buffer.str(), ec);
	}
}

// With string
Bruc::Bruc(std::string data) {
	error_code ec;
	*this = Bruc(data, ec);
}

Bruc::Bruc(std::string data, error_code& ec) { parse(data, ec); }

// Auxiliar class to parse the text
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
};

void Bruc::parse(std::string data, Bruc::error_code& ec) {
	seccions.clear();
	// Create initial values
	std::string s_name = "__global__";
	std::unordered_map<std::string, std::string> s_pairs;

	// Start the parser
	Parser p(data);
	while (p.getNextLine()) {
		// Start a new section
		if (p.lineIsSection()) {
			// Add previous name and pairs to the map
			seccions[s_name] = s_pairs;
			// Clear the pairs and update the name
			s_pairs.clear();
			s_name = p.lineSectionName();
			// Check if s_name already exists
			if (seccions.find(s_name) != seccions.end()) ec = error_code(error_code::SectionExists, p.ln(), s_name);
			else continue;
			return;
		}

		// It's a key - value pair
		auto [key, value] = p.lineKeyValue();

		// Check pair is valid
		if (key.empty() || value.empty ()) {
			if (key.empty() && value.empty()) ec = error_code(error_code::WrongPairFormat, p.ln(), s_name);
			else if (key.empty()) ec = error_code(error_code::EmptyKeyName, p.ln(), s_name);
			else ec = error_code(error_code::EmptyKeyValue, p.ln(), s_name, key);
			return;
		}
		// Check key is new
		if (s_pairs.find(key) != s_pairs.end()) {
			ec = error_code(error_code::KeyExists, p.ln(), s_name, key);
			return;
		}
		// Add the pair
		s_pairs[key] = value;
	}
	// End of file, so add the pairs to the section
	seccions[s_name] = s_pairs;
}

