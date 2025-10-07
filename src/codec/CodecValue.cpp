#include "CodecValue.h"

namespace codec {
struct CodecValueComparator {
    const CodecValue& rhs; // The value being compared against

    bool operator()(const SimpleString& lhs) const
    {
        return std::holds_alternative<SimpleString>(rhs.data) && lhs.value == std::get<SimpleString>(rhs.data).value;
    }

    bool operator()(const Error& lhs) const
    {
        return std::holds_alternative<Error>(rhs.data) && lhs.value == std::get<Error>(rhs.data).value;
    }

    bool operator()(const Integer& lhs) const
    {
        return std::holds_alternative<Integer>(rhs.data) && lhs.value == std::get<Integer>(rhs.data).value;
    }

    bool operator()(const BulkString& lhs) const
    {
        return std::holds_alternative<BulkString>(rhs.data) && lhs.value == std::get<BulkString>(rhs.data).value;
    }

    bool operator()(const Array& lhs) const
    {
        if (!std::holds_alternative<Array>(rhs.data)) {
            return false;
        }
        const auto& rhs_array = std::get<Array>(rhs.data);
        if (lhs.elements.size() != rhs_array.elements.size()) {
            return false;
        }
        for (size_t i = 0; i < lhs.elements.size(); ++i) {
            // Recursive check
            if (!(lhs.elements[i] == rhs_array.elements[i])) {
                return false;
            }
        }
        return true;
    }
};

bool operator==(const CodecValue& lhs, const CodecValue& rhs)
{
    return std::visit(CodecValueComparator { rhs }, lhs.data);
}
} // namespace codec
