#include "BrucError.h"

class BrucErrorCategoryImpl : public std::error_category {
public:
    const char* name() const noexcept override {
        return "bruc_error";
    }

    std::string message(int ev) const override {
        switch (static_cast<BrucError>(ev)) {
            case BrucError::SectionExists:   return "Section already exists";
            case BrucError::KeyExists:       return "Key already exists";
            case BrucError::EmptyKeyValue:   return "Empty key value";
            case BrucError::EmptyKeyName:    return "Empty key name";
            case BrucError::WrongPairFormat: return "Wrong pair format";
            case BrucError::WrongTypeFormat: return "Wrong type format";
            default:                        return "Unknown bruc error";
        }
    }
};

const std::error_category& bruc_error_category() {
    static BrucErrorCategoryImpl instance;
    return instance;
}

std::error_code make_error_code(BrucError e) {
    return {static_cast<int>(e), bruc_error_category()};
}

