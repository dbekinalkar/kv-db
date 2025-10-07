#include "SortedSetCommands.h"
#include "CommandHelpers.h"

namespace command {
codec::CodecValue cmdZAdd(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 4)
        return codec::err("ERR wrong number of arguments for 'zadd' command");
    try {
        std::string key = extractBulkString(args[1]);
        double score = parseDouble(extractBulkString(args[2]));
        std::string member = extractBulkString(args[3]);
        size_t added = store.zadd(key, score, member);
        return codec::integer(added);
    } catch (const std::exception& e) {
        return codec::err(std::string("ERR ") + e.what());
    }
}

codec::CodecValue cmdZRem(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 3)
        return codec::err("ERR wrong number of arguments for 'zrem' command");
    try {
        std::string key = extractBulkString(args[1]);
        std::string member = extractBulkString(args[2]);
        size_t removed = store.zrem(key, member);
        return codec::integer(removed);
    } catch (const std::exception& e) {
        return codec::err(std::string("ERR ") + e.what());
    }
}

codec::CodecValue cmdZRange(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 4)
        return codec::err("ERR wrong number of arguments for 'zrange' command");
    try {
        std::string key = extractBulkString(args[1]);
        int start = parseInteger(extractBulkString(args[2]));
        int stop = parseInteger(extractBulkString(args[3]));
        std::vector<std::string> result = store.zrange(key, start, stop);
        std::vector<codec::CodecValue> values;
        for (const auto& s : result) {
            values.push_back(codec::bulk(s));
        }
        return codec::array(values);
    } catch (const std::exception& e) {
        return codec::err(std::string("ERR ") + e.what());
    }
}
}