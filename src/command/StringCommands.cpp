#include "StringCommands.h"
#include "CommandHelpers.h"

#include "Codec.h"
#include "KeyValueStore.h"

namespace command {
codec::CodecValue cmdSet(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 3)
        return codec::err("ERR wrong number of arguments for 'set' command");
    try {
        std::string key = extractBulkString(args[1]);
        std::string value = extractBulkString(args[2]);
        store.set(key, value);
        return codec::ok();
    } catch (const std::exception& e) {
        return codec::err(std::string("ERR ") + e.what());
    }
}

codec::CodecValue cmdGet(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 2)
        return codec::err("ERR wrong number of arguments for 'get' command");
    try {
        std::string key = extractBulkString(args[1]);
        std::string value = store.get(key);
        return codec::bulk(value);
    } catch (const std::exception&) {
        return codec::nullBulk();
    }
}

codec::CodecValue cmdDel(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 2)
        return codec::err("ERR wrong number of arguments for 'del' command");
    try {
        std::string key = extractBulkString(args[1]);
        bool deleted = store.del(key);
        return codec::integer(deleted ? 1 : 0);
    } catch (const std::exception& e) {
        return codec::err(std::string("ERR ") + e.what());
    }
}

codec::CodecValue cmdExists(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args)
{
    if (args.size() != 2)
        return codec::err("ERR wrong number of arguments for 'exists' command");
    try {
        std::string key = extractBulkString(args[1]);
        bool exists = store.exists(key);
        return codec::integer(exists ? 1 : 0);
    } catch (const std::exception& e) {
        return codec::err(std::string("ERR ") + e.what());
    }
}
} // namespace command
