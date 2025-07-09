#include "Bruc.h"
#include <algorithm>

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
T parseValue(std::string value, int& err) {
	value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](char c) { return !std::isspace(c); }));
	value.erase(std::find_if(value.rbegin(), value.rend(), [](char c) { return !std::isspace(c); }).base(), value.end());

	T result;
	err = 0;

	if constexpr (std::is_same_v<T, std::string>){
		if (!value.empty() && value.front() == '"' && value.back() == '"')
			return std::string(value.begin() + 1, value.end() - 1);
		return value;
	} else if constexpr (std::is_same_v<T, std::filesystem::path>) {
		return std::filesystem::path(parseValue<std::string>(value, err));

	} else if constexpr (is_vector_v<T>) {
		using A = typename T::value_type;
		if (!value.empty() && value.front() == '{' && value.back() == '}') {
			std::istringstream ss(std::string(value.begin()+1, value.end()-1));
			std::string item;
			while (std::getline(ss, item, ',')) 
				result.push_back(parseValue<A>(item, err));

		} else if (value.size() >= 5 && value.front() == '(' && value.back() == ')') {
			auto comma = value.begin();
			for (; *comma != ',' && comma != value.end(); ++comma);
			unsigned int N = parseValue<unsigned int>(std::string(value.begin()+1, comma), err);
			A M = parseValue<A>(std::string(comma+1, value.end()-1), err);
			for (int i = 0; i < N; ++i) result.push_back(M);
		} else err = 1;
		return result;
	} else {
		std::istringstream converter(value);
		if (value.rfind("0x", 0) == 0 || value.rfind("0X", 0) == 0)
			converter >> std::hex;
		if(!(converter >> result)) err = 1;
		return result;
	}
}

template <typename T>
T Bruc::get(std::string s, std::string k, std::string d) {
	error_code ec;
	return get<T>(s, k, d, ec);
}

template <typename T>
T Bruc::get(std::string s, std::string k, error_code& ec) {
	return get<T>(s, k, "", ec);
}

template <typename T>
T Bruc::get(std::string s, std::string k, std::string d, error_code& ec) 
{

	int err;
	T ret;

	if (!exists(s,k)) {
		ret = parseValue<T>(d, err);
		if (err) ec = error_code(error_code::WrongTypeFormat, s, k);
	} else {

		std::string value = seccions.at(s).at(k);
		ret = parseValue<T>(value, err);
		if (err) ec = error_code(error_code::WrongTypeFormat, s, k);
	}
	return ret;
}

