#include <string>
#include <vector>
#include <unordered_map>

class Bruc {
	private:
		// Variables
		std::unordered_map<std::string, std::unordered_map<std::string, std::string>> seccions;

	public:
		// Returns the value interpreted as the valid types
		template <typename T = std::string>
		T get(std::string s, std::string k);

		// Returns true if section s exists
		bool exists(std::string s);
		
		// Returns true if key k exists in section s
		bool exists(std::string s, std::string k);

		// Helper function for error tracking returns if the map is valid
		bool error();

		// Returns a vector with all the sections
		std::vector<std::string> getSections();

		// Returns the list of keys in section s 
		std::vector<std::string> getKeys(std::string s);

		// Creator function from file
		static Bruc readFile(std::string file);

		// Creator function from string
		static Bruc readString(std::string data);
};
