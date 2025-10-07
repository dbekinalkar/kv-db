#include "ListCommands.h"
#include "CommandHelpers.h"

namespace command {
codec::CodecValue cmdLPush(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 3)
        return codec::err("ERR wrong number of arguments for 'lpush' command");
    try {
        std::string key = extractBulkString(args[1]);
        std::string value = extractBulkString(args[2]);
        size_t len = store.lpush(key, value);
        return codec::integer(len);
    } catch (const std::exception& e) {
        return codec::err(std::string("ERR ") + e.what());
    }
}

codec::CodecValue cmdRPush(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 3)
        return codec::err("ERR wrong number of arguments for 'rpush' command");
    try {
        std::string key = extractBulkString(args[1]);
        std::string value = extractBulkString(args[2]);
        size_t len = store.rpush(key, value);
        return codec::integer(len);
    } catch (const std::exception& e) {
        return codec::err(std::string("ERR ") + e.what());
    }
}

codec::CodecValue cmdLPop(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 2)
        return codec::err("ERR wrong number of arguments for 'lpop' command");
    try {
        std::string key = extractBulkString(args[1]);
        std::string value = store.lpop(key);
        return codec::bulk(value);
    } catch (const std::exception&) {
        return codec::nullBulk();
    }
}

codec::CodecValue cmdRPop(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 2)
        return codec::err("ERR wrong number of arguments for 'rpop' command");
    try {
        std::string key = extractBulkString(args[1]);
        std::string value = store.rpop(key);
        return codec::bulk(value);
    } catch (const std::exception&) {
        return codec::nullBulk();
    }
}

codec::CodecValue cmdLRange(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 4)
        return codec::err("ERR wrong number of arguments for 'lrange' command");
    try {
        std::string key = extractBulkString(args[1]);
        int start = parseInteger(extractBulkString(args[2]));
        int stop = parseInteger(extractBulkString(args[3]));
        std::vector<std::string> result = store.lrange(key, start, stop);
        std::vector<codec::CodecValue> values;
        for (const auto& s : result) {
            values.push_back(codec::bulk(s));
        }
        return codec::array(values);
    } catch (const std::exception& e) {
        return codec::err(std::string("ERR ") + e.what());
    }
}
} // namespace command
