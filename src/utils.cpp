#include "Bruc.h"

// Simple getters
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

std::vector<std::string> Bruc::sections() {
	std::vector<std::string> list;
	for (const auto& [k, _] : seccions)
		list.push_back(k);
	return list;
}

std::vector<std::string> Bruc::keys(std::string s) {
	std::vector<std::string> list;
	if (!exists(s)) return list;

	for (const auto& [k, _] : seccions.at(s))
		list.push_back(k);
	return list;
}

