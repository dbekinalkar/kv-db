#include "CommandProcessor.h"
#include "CommandHelpers.h"
#include "HashCommands.h"
#include "ListCommands.h"
#include "SetCommands.h"
#include "SortedSetCommands.h"
#include "StringCommands.h"
#include <algorithm>
#include <functional>
#include <unordered_map>

namespace command {

using CommandHandler = std::function<codec::CodecValue(storage::KeyValueStore&, const std::vector<codec::CodecValue>&)>;

const std::unordered_map<std::string, CommandHandler> commandMap = {
    { "SET", cmdSet },
    { "GET", cmdGet },
    { "DEL", cmdDel },
    { "EXISTS", cmdExists },
    { "LPUSH", cmdLPush },
    { "RPUSH", cmdRPush },
    { "LPOP", cmdLPop },
    { "RPOP", cmdRPop },
    { "LRANGE", cmdLRange },
    { "SADD", cmdSAdd },
    { "SREM", cmdSRem },
    { "SMEMBERS", cmdSMembers },
    { "HSET", cmdHSet },
    { "HGET", cmdHGet },
    { "HDEL", cmdHDel },
    { "HGETALL", cmdHGetAll },
    { "ZADD", cmdZAdd },
    { "ZREM", cmdZRem },
    { "ZRANGE", cmdZRange }
};

CommandProcessor::CommandProcessor(storage::KeyValueStore& kvStore)
    : kvStore_(kvStore)
{
}

codec::CodecValue CommandProcessor::process(const codec::CodecValue& msg) const
{
    // Extract array from message
    const codec::Array* arr = std::get_if<codec::Array>(&msg.data);
    if (!arr || arr->elements.empty()) {
        return codec::err("ERR empty command");
    }

    // Extract command name
    std::string command;
    try {
        command = toUpper(extractBulkString(arr->elements[0]));
    } catch (const std::exception& e) {
        return codec::err("ERR invalid command format");
    }

    // Look up and execute command
    auto it = commandMap.find(command);
    if (it != commandMap.end()) {
        return it->second(kvStore_, arr->elements);
    }

    return codec::err("ERR unknown command '" + command + "'");
}

} // namespace command