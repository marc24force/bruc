#define BRUC_INSTANTIATE_GET(T) template T Bruc::get<T>(std::string, std::string); \
	                        template std::vector<T> Bruc::get<std::vector<T>>(std::string, std::string)

BRUC_INSTANTIATE_GET(bool);
BRUC_INSTANTIATE_GET(int);
BRUC_INSTANTIATE_GET(unsigned int);
BRUC_INSTANTIATE_GET(long);
BRUC_INSTANTIATE_GET(unsigned long);
BRUC_INSTANTIATE_GET(float);
BRUC_INSTANTIATE_GET(double);
BRUC_INSTANTIATE_GET(std::string);

template<typename>
struct is_vector : std::false_type {};

template<typename T, typename A>
struct is_vector<std::vector<T, A>> : std::true_type {};

template<typename T>
constexpr bool is_vector_v = is_vector<T>::value;

template <typename T>
T parseValue(std::string value) {
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
				result.push_back(parseValue<A>(item));

		} else if (value.size() >= 5 && value.front() == '(' && value.back() == ')') {
			auto comma = value.begin();
			for (; *comma != ',' && comma != value.end(); ++comma);
			unsigned int N = std::stoul(std::string(value.begin()+1, comma));
			A M = parseValue<A>(std::string(comma+1, value.end()-1));
			for (int i = 0; i < N; ++i) result.push_back(M);
		}
		return result;
	} else {
		std::istringstream converter(value);
		converter >> result;
		return result;
	}
}
