#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace storage {

using RedisString = std::string;
using RedisList = std::vector<std::string>;
using RedisSet = std::unordered_set<std::string>;
using RedisHash = std::unordered_map<std::string, std::string>;
using RedisZSet = std::map<double, std::string>;

using RedisVariant = std::variant<
    std::monostate,
    RedisString,
    RedisList,
    RedisSet,
    RedisHash,
    RedisZSet>;

} // namespace storage
