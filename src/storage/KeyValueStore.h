#pragma once

#include "StorageTypes.h"
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace storage {

class KeyValueStore {
public:
    // String operations
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    bool del(const std::string& key);
    bool exists(const std::string& key);

    // List operations
    size_t lpush(const std::string& key, const std::string& value);
    size_t rpush(const std::string& key, const std::string& value);
    std::string lpop(const std::string& key);
    std::string rpop(const std::string& key);
    std::vector<std::string> lrange(const std::string& key, int start, int stop);

    // Set operations
    size_t sadd(const std::string& key, const std::string& member);
    size_t srem(const std::string& key, const std::string& member);
    std::unordered_set<std::string> smembers(const std::string& key);

    // Hash operations
    bool hset(const std::string& key, const std::string& field, const std::string& value);
    std::string hget(const std::string& key, const std::string& field);
    bool hdel(const std::string& key, const std::string& field);
    std::unordered_map<std::string, std::string> hgetall(const std::string& key);

    // Sorted Set operations
    size_t zadd(const std::string& key, double score, const std::string& member);
    size_t zrem(const std::string& key, const std::string& member);
    std::vector<std::string> zrange(const std::string& key, int start, int stop);

private:
    std::unordered_map<std::string, RedisVariant> store_;

    template <typename T>
    T& getOrCreate(const std::string& key);

    template <typename T>
    T& getOrThrow(const std::string& key);
};

} // namespace storage
