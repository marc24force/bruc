#include <iostream>
#include <vector>
#include <filesystem>
#include "Bruc.h"  // Assume Bruc class is declared here


int main() {
    Bruc config = Bruc::readFile("example/example.bruc");

    if (config.error()) {
        std::cerr << "Error reading config\n";
        return 1;
    }

    if (!config.exists("General")) {
        std::cerr << "Section General missing\n";
        return 1;
    }

    std::cout << std::endl;

    // Test getting various types
    int ivalue = config.get<int>("General", "int_value");
    unsigned int uivalue = config.get<unsigned int>("General", "uint_value");
    long lvalue = config.get<long>("General", "long_value");
    unsigned long ulvalue = config.get<unsigned long>("General", "ulong_value");
    float fvalue = config.get<float>("General", "float_value");
    double dvalue = config.get<double>("General", "double_value");
    std::string strvalue = config.get<std::string>("General", "string_value");
    std::filesystem::path pvalue = config.get<std::filesystem::path>("General", "path_value");
    bool bvalue_true = config.get<bool>("General", "bool_true");
    bool bvalue_false = config.get<bool>("General", "bool_false");

    std::cout << "int_value: " << ivalue << "\n";
    std::cout << "uint_value: " << uivalue << "\n";
    std::cout << "long_value: " << lvalue << "\n";
    std::cout << "ulong_value: " << ulvalue << "\n";
    std::cout << "float_value: " << fvalue << "\n";
    std::cout << "double_value: " << dvalue << "\n";
    std::cout << "string_value: " << strvalue << "\n";
    std::cout << "path_value: " << pvalue << "\n";
    std::cout << "bool_true: " << std::boolalpha << bvalue_true << "\n";
    std::cout << "bool_false: " << std::boolalpha << bvalue_false << "\n";
    std::cout << "no_value: " << config.get("General", "no_value", "value doesn't exist") << "\n";

    std::cout << std::endl;

    if (!config.exists("Arrays")) {
        std::cerr << "Section Arrays missing\n";
        return 1;
    }

    std::vector<int> int_vec = config.get<std::vector<int>>("Arrays", "int_vec");
    std::cout << "int_vec: "; for (auto v : int_vec) std::cout << v << ", "; std::cout << "\n";
    std::vector<float> float_vec = config.get<std::vector<float>>("Arrays", "float_vec");
    std::cout << "float_vec: "; for (auto v : float_vec) std::cout << v << ", "; std::cout << "\n";
    std::vector<std::string> string_vec = config.get<std::vector<std::string>>("Arrays", "string_vec");
    std::cout << "string_vec: "; for (auto v : string_vec) std::cout << v << ", "; std::cout << "\n";
    std::vector<bool> bool_vec = config.get<std::vector<bool>>("Arrays", "bool_vec");
    std::cout << "bool_vec: "; for (auto v : bool_vec) std::cout << v << ", "; std::cout << "\n";
    std::vector<std::string> empty_vec = config.get<std::vector<std::string>>("Arrays", "empty_vec");
    std::cout << "empty_vec: {"; for (auto v : empty_vec) std::cout << v << ", "; std::cout << "}\n";

    std::cout << std::endl;

    std::vector<int> int_array = config.get<std::vector<int>>("Arrays", "int_array");
    std::cout << "int_array: "; for (auto v : int_array) std::cout << v << ", "; std::cout << "\n";
    std::vector<float> float_array = config.get<std::vector<float>>("Arrays", "float_array");
    std::cout << "float_array: "; for (auto v : float_array) std::cout << v << ", "; std::cout << "\n";
    std::vector<std::string> string_array = config.get<std::vector<std::string>>("Arrays", "string_array");
    std::cout << "string_array: "; for (auto v : string_array) std::cout << v << ", "; std::cout << "\n";
    std::vector<bool> bool_array = config.get<std::vector<bool>>("Arrays", "bool_array");
    std::cout << "bool_array: "; for (auto v : bool_array) std::cout << v << ", "; std::cout << "\n";

    std::cout << std::endl;


    // List sections
    auto sections = config.getSections();
    std::cout << "Sections:\n";
    for (const auto& sec : sections)
        std::cout << " - " << sec << "\n";
    std::cout << std::endl;

    // List keys in General
    for (const auto& sec : sections) {
	    auto keys = config.getKeys(sec);
	    std::cout << "Keys in " << sec << ":\n";
	    for (const auto& key : keys)
		    std::cout << " * " << key << "\n";
	    std::cout << std::endl;
    }

    return 0;
}

