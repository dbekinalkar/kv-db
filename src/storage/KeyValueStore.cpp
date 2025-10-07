#include "KeyValueStore.h"

namespace storage {

// String operations
void KeyValueStore::set(const std::string& key, const std::string& value)
{
    store_[key] = RedisString(value);
}

std::string KeyValueStore::get(const std::string& key)
{
    auto it = store_.find(key);
    if (it == store_.end() || !std::holds_alternative<RedisString>(it->second))
        throw std::runtime_error("Key not found or wrong type");
    return std::get<RedisString>(it->second);
}

bool KeyValueStore::del(const std::string& key)
{
    return store_.erase(key) > 0;
}

bool KeyValueStore::exists(const std::string& key)
{
    return store_.find(key) != store_.end();
}

// List operations
size_t KeyValueStore::lpush(const std::string& key, const std::string& value)
{
    auto& list = getOrCreate<RedisList>(key);
    list.insert(list.begin(), value);
    return list.size();
}

size_t KeyValueStore::rpush(const std::string& key, const std::string& value)
{
    auto& list = getOrCreate<RedisList>(key);
    list.push_back(value);
    return list.size();
}

std::string KeyValueStore::lpop(const std::string& key)
{
    auto& list = getOrThrow<RedisList>(key);
    if (list.empty())
        throw std::runtime_error("List is empty");
    std::string val = list.front();
    list.erase(list.begin());
    return val;
}

std::string KeyValueStore::rpop(const std::string& key)
{
    auto& list = getOrThrow<RedisList>(key);
    if (list.empty())
        throw std::runtime_error("List is empty");
    std::string val = list.back();
    list.pop_back();
    return val;
}

std::vector<std::string> KeyValueStore::lrange(const std::string& key, int start, int stop)
{
    auto& list = getOrThrow<RedisList>(key);
    int size = static_cast<int>(list.size());
    if (start < 0)
        start += size;
    if (stop < 0)
        stop += size;
    if (start < 0)
        start = 0;
    if (stop >= size)
        stop = size - 1;
    if (start > stop || start >= size)
        return {};
    return std::vector<std::string> { list.begin() + start, list.begin() + stop + 1 };
}

// Set operations
size_t KeyValueStore::sadd(const std::string& key, const std::string& member)
{
    auto& set = getOrCreate<RedisSet>(key);
    size_t curr_size = set.size();
    auto [_, inserted] = set.insert(member);
    return set.size() - curr_size;
}

size_t KeyValueStore::srem(const std::string& key, const std::string& member)
{
    auto& set = getOrThrow<RedisSet>(key);
    std::size_t curr_size = set.size();
    set.erase(member);
    return curr_size - set.size();
}

std::unordered_set<std::string> KeyValueStore::smembers(const std::string& key)
{
    auto& set = getOrThrow<RedisSet>(key);
    return set;
}

// Hash operations
bool KeyValueStore::hset(const std::string& key, const std::string& field, const std::string& value)
{
    auto& hash = getOrCreate<RedisHash>(key);
    bool is_new = hash.find(field) == hash.end();
    hash[field] = value;
    return is_new;
}

std::string KeyValueStore::hget(const std::string& key, const std::string& field)
{
    auto& hash = getOrThrow<RedisHash>(key);
    auto it = hash.find(field);
    if (it == hash.end())
        throw std::runtime_error("Field not found");
    return it->second;
}

bool KeyValueStore::hdel(const std::string& key, const std::string& field)
{
    auto& hash = getOrThrow<RedisHash>(key);
    return hash.erase(field) > 0;
}

std::unordered_map<std::string, std::string> KeyValueStore::hgetall(const std::string& key)
{
    auto& hash = getOrThrow<RedisHash>(key);
    return hash;
}

// Sorted Set operations
size_t KeyValueStore::zadd(const std::string& key, double score, const std::string& member)
{
    auto& zset = getOrCreate<RedisZSet>(key);
    std::size_t curr_size = zset.size();
    // Remove existing member if present
    for (auto it = zset.begin(); it != zset.end();) {
        if (it->second == member)
            it = zset.erase(it);
        else
            ++it;
    }
    zset.emplace(score, member);
    return zset.size() - curr_size;
}

size_t KeyValueStore::zrem(const std::string& key, const std::string& member)
{
    auto& zset = getOrThrow<RedisZSet>(key);
    size_t removed = 0;
    for (auto it = zset.begin(); it != zset.end();) {
        if (it->second == member) {
            it = zset.erase(it);
            ++removed;
        } else {
            ++it;
        }
    }
    return removed;
}

std::vector<std::string> KeyValueStore::zrange(const std::string& key, int start, int stop)
{
    auto& zset = getOrThrow<RedisZSet>(key);
    int size = static_cast<int>(zset.size());
    if (start < 0)
        start += size;
    if (stop < 0)
        stop += size;
    if (start < 0)
        start = 0;
    if (stop >= size)
        stop = size - 1;
    if (start > stop || start >= size)
        return {};
    std::vector<std::string> result;
    int idx = 0;
    for (const auto& [score, member] : zset) {
        if (idx >= start && idx <= stop)
            result.push_back(member);
        if (idx > stop)
            break;
        ++idx;
    }
    return result;
}

// Helpers
template <typename T>
T& KeyValueStore::getOrCreate(const std::string& key)
{
    auto it = store_.find(key);
    if (it == store_.end() || !std::holds_alternative<T>(it->second)) {
        store_[key] = T();
    }
    return std::get<T>(store_[key]);
}

template <typename T>
T& KeyValueStore::getOrThrow(const std::string& key)
{
    auto it = store_.find(key);
    if (it == store_.end() || !std::holds_alternative<T>(it->second))
        throw std::runtime_error("Key not found or wrong type");
    return std::get<T>(it->second);
}

template RedisString& KeyValueStore::getOrCreate<RedisString>(const std::string&);
template RedisList& KeyValueStore::getOrCreate<RedisList>(const std::string&);
template RedisSet& KeyValueStore::getOrCreate<RedisSet>(const std::string&);
template RedisHash& KeyValueStore::getOrCreate<RedisHash>(const std::string&);
template RedisZSet& KeyValueStore::getOrCreate<RedisZSet>(const std::string&);

template RedisString& KeyValueStore::getOrThrow<RedisString>(const std::string&);
template RedisList& KeyValueStore::getOrThrow<RedisList>(const std::string&);
template RedisSet& KeyValueStore::getOrThrow<RedisSet>(const std::string&);
template RedisHash& KeyValueStore::getOrThrow<RedisHash>(const std::string&);
template RedisZSet& KeyValueStore::getOrThrow<RedisZSet>(const std::string&);

}; // namespace storage