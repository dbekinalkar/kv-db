#include "SetCommands.h"
#include "CommandHelpers.h"

namespace command {
codec::CodecValue cmdSAdd(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 3)
        return codec::err("ERR wrong number of arguments for 'sadd' command");
    try {
        std::string key = extractBulkString(args[1]);
        std::string member = extractBulkString(args[2]);
        size_t added = store.sadd(key, member);
        return codec::integer(added);
    } catch (const std::exception& e) {
        return codec::err(std::string("ERR ") + e.what());
    }
}

codec::CodecValue cmdSRem(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 3)
        return codec::err("ERR wrong number of arguments for 'srem' command");
    try {
        std::string key = extractBulkString(args[1]);
        std::string member = extractBulkString(args[2]);
        size_t removed = store.srem(key, member);
        return codec::integer(removed);
    } catch (const std::exception& e) {
        return codec::err(std::string("ERR ") + e.what());
    }
}

codec::CodecValue cmdSMembers(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 2)
        return codec::err("ERR wrong number of arguments for 'smembers' command");
    try {
        std::string key = extractBulkString(args[1]);
        auto members = store.smembers(key);
        std::vector<codec::CodecValue> values;
        for (const auto& m : members) {
            values.push_back(codec::bulk(m));
        }
        return codec::array(values);
    } catch (const std::exception& e) {
        return codec::err(std::string("ERR ") + e.what());
    }
}
}