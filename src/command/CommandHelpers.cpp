#include "CommandHelpers.h"

#include "Codec.h"
#include <algorithm>
#include <stdexcept>
#include <string>

namespace command {
std::string toUpper(const std::string& str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string extractBulkString(const codec::CodecValue& val)
{
    if (auto* bulk = std::get_if<codec::BulkString>(&val.data)) {
        if (bulk->value) {
            return *bulk->value;
        }
        throw std::runtime_error("Null bulk string");
    }
    throw std::runtime_error("Expected bulk string");
}

long long parseInteger(const std::string& str)
{
    return std::stoll(str);
}

double parseDouble(const std::string& str)
{
    return std::stod(str);
}
} // namespace command