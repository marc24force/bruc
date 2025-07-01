#include <iostream>
#include <vector>
#include <filesystem>
#include "Bruc.h"

Bruc b;

template <typename T>
T get (std::string s, std::string k, std::string d="") {
	T r = b.get<T>(s,k,d);
	if (b.error()) {
		std::cerr << "Error: " << b.error().message() << std::endl;
		exit(1);
	}
	return r;
}

auto print_vec = [](const auto& vec) {
	std::cout << "{";
	for (size_t i = 0; i < vec.size(); ++i) {
		std::cout << vec[i];
		if (i + 1 < vec.size()) std::cout << ",";
	}
	std::cout << "}";
};




int main() {
	b = Bruc::readFile("example/example.bruc");

	if (b.error()) {
		std::cerr << "Error: " << b.error().message() << ". Line " << b.getErrorLine() << std::endl;
		return 1;
	}

	std::cout << std::endl;

	// Test getting various types
	std::cout << "int_value: " << get<int>("General", "int_value") << "\n";
	std::cout << "uint_value: " << get<unsigned int> ("General", "uint_value") << "\n";
	std::cout << "long_value: " << get<long>("General", "long_value") << "\n";
	std::cout << "ulong_value: " << get<unsigned long>("General", "ulong_value") << "\n";
	std::cout << "float_value: " << get<float>("General", "float_value") << "\n";
	std::cout << "double_value: " << get<double>("General", "double_value") << "\n";
	std::cout << "string_value: " << get<std::string>("General", "string_value") << "\n";
	std::cout << "path_value: " << get<std::filesystem::path>("General", "path_value") << "\n";
	std::cout << "bool_value: " << get<bool>("General", "bool_value") << "\n";

	std::cout << "no_value: " << b.get("General", "no_value", "no_value doesn't exist") << "\n";

	std::cout << std::endl;

	std::cout << "int_vec: "; print_vec(get<std::vector<int>>("Arrays", "int_vec")); std::cout << "\n";
	std::cout << "float_vec: "; print_vec(get<std::vector<float>>("Arrays", "float_vec")); std::cout << "\n";
	std::cout << "string_vec: "; print_vec(get<std::vector<std::string>>("Arrays", "string_vec")); std::cout << "\n";
	std::cout << "bool_vec: "; print_vec(get<std::vector<bool>>("Arrays", "bool_vec")); std::cout << "\n";


	std::cout << std::endl;

	std::cout << "int_array: "; print_vec(get<std::vector<int>>("Arrays", "int_array")); std::cout << "\n";
	std::cout << "float_array: "; print_vec(get<std::vector<float>>("Arrays", "float_array")); std::cout << "\n";
	std::cout << "string_array: "; print_vec(get<std::vector<std::string>>("Arrays", "string_array")); std::cout << "\n";
	std::cout << "bool_array: "; print_vec(get<std::vector<bool>>("Arrays", "bool_array")); std::cout << "\n";

	std::cout << std::endl;


	// List sections
	auto sections = b.getSections();
	std::cout << "Sections:\n";
	for (const auto& sec : sections)
		std::cout << " - " << sec << "\n";
	std::cout << std::endl;

	// List keys in all sections
	for (const auto& sec : sections) {
		auto keys = b.getKeys(sec);
		std::cout << "Keys in " << sec << ":\n";
		for (const auto& key : keys)
			std::cout << " * " << key << "\n";
		std::cout << std::endl;
	}

	return 0;
}

