#include "HashCommands.h"

#include "CommandHelpers.h"

namespace command {

// Hash commands
codec::CodecValue cmdHSet(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 4)
        return codec::err("ERR wrong number of arguments for 'hset' command");
    try {
        std::string key = extractBulkString(args[1]);
        std::string field = extractBulkString(args[2]);
        std::string value = extractBulkString(args[3]);
        bool added = store.hset(key, field, value);
        return codec::integer(added ? 1 : 0);
    } catch (const std::exception& e) {
        return codec::err(std::string("ERR ") + e.what());
    }
}

codec::CodecValue cmdHGet(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 3)
        return codec::err("ERR wrong number of arguments for 'hget' command");
    try {
        std::string key = extractBulkString(args[1]);
        std::string field = extractBulkString(args[2]);
        std::string value = store.hget(key, field);
        return codec::bulk(value);
    } catch (const std::exception&) {
        return codec::nullBulk();
    }
}

codec::CodecValue cmdHDel(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 3)
        return codec::err("ERR wrong number of arguments for 'hdel' command");
    try {
        std::string key = extractBulkString(args[1]);
        std::string field = extractBulkString(args[2]);
        bool deleted = store.hdel(key, field);
        return codec::integer(deleted ? 1 : 0);
    } catch (const std::exception& e) {
        return codec::err(std::string("ERR ") + e.what());
    }
}

codec::CodecValue cmdHGetAll(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 2)
        return codec::err("ERR wrong number of arguments for 'hgetall' command");
    try {
        std::string key = extractBulkString(args[1]);
        auto hash = store.hgetall(key);
        std::vector<codec::CodecValue> values;
        for (const auto& [field, value] : hash) {
            values.push_back(codec::bulk(field));
            values.push_back(codec::bulk(value));
        }
        return codec::array(values);
    } catch (const std::exception& e) {
        return codec::err(std::string("ERR ") + e.what());
    }
}

}