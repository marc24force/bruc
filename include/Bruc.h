#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "BrucError.h"


class Bruc {
	private:
		// Variables
		std::unordered_map<std::string, std::unordered_map<std::string, std::string>> seccions;
		std::error_code _error;
		unsigned int _error_line;

		// Error constructor
		Bruc(std::error_code ec, unsigned int ln = 0);

	public:
		// Normal constructor
		Bruc();

		// Returns the value interpreted as the valid types
		template <typename T = std::string>
		T get(std::string s, std::string k, std::string d = "");

		// Returns true if section s exists
		bool exists(std::string s);
		
		// Returns true if key k exists in section s
		bool exists(std::string s, std::string k);

		// Helper function for error tracking returns if the map is valid
		std::error_code error();
		unsigned int getErrorLine();

		// Returns a vector with all the sections
		std::vector<std::string> getSections();

		// Returns the list of keys in section s 
		std::vector<std::string> getKeys(std::string s);

		// Creator function from file
		static Bruc readFile(std::string file);

		// Creator function from string
		static Bruc readString(std::string data);
};
