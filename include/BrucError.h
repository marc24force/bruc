#pragma once

#include <system_error>

/* Error handling with std::error_code */
enum class BrucError {
	SectionExists = 1,
	KeyExists,
	EmptyKeyValue,
	EmptyKeyName,
	WrongPairFormat,
	WrongTypeFormat
};

const std::error_category& bruc_error_category();

std::error_code make_error_code(BrucError e);

namespace std {
	template<> struct is_error_code_enum<BrucError> : true_type {};
}

