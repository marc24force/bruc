#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>

class Bruc {
	public:
		// Error code
		class error_code {
			public:
				enum error_type {
					NoError = 0,
					SectionExists = 1,
					KeyExists,
					EmptyKeyValue,
					EmptyKeyName,                                    
					WrongPairFormat,    
					WrongTypeFormat
				};

			private: 
				// Variables
				error_type _code;
				bool _general;
				int _line;
				std::string _section;
				std::string _key;
			public:
				error_code(int code = 0, bool general = false);
				error_code(int code, std::string s, std::string k = "");
				error_code(int code, int l, std::string s, std::string k = "");

				explicit operator bool() const;

				std::string message() const;
				std::string verbose() const;
				int value() const;
				int line() const;
				std::string section() const;
				std::string key() const;
		};

	private:
		// Variables
		std::unordered_map<std::string, std::unordered_map<std::string, std::string>> seccions;

		// Parser method
		void parse(std::string data, error_code& ec);

	public:
		// Empty constructor
		Bruc() {}

		// Constructor with a path
		Bruc(std::filesystem::path file);
		Bruc(std::filesystem::path file, error_code& ec);

		// Constructor with a string
		Bruc(std::string data);
		Bruc(std::string data, error_code& ec);

		// Returns the value interpreted as the valid types, no error code
		template <typename T = std::string>
			T get(std::string s, std::string k, std::string d = "");

		// Returns the value interpreted as the valid types, error code but no default
		template <typename T = std::string>
			T get(std::string s, std::string k, error_code& ec);

		// Returns the value interpreted as the valid types, error code and default
		template <typename T = std::string>
			T get(std::string s, std::string k, std::string d, error_code& ec);

		// Returns true if section s exists
		bool exists(std::string s);

		// Returns true if key k exists in section s
		bool exists(std::string s, std::string k);

		// Returns a vector with all the sections
		std::vector<std::string> sections();

		// Returns the list of keys in section s 
		std::vector<std::string> keys(std::string s);

};
