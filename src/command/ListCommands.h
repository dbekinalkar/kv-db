#pragma once

#include "Codec.h"
#include "KeyValueStore.h"

namespace command {

codec::CodecValue cmdLPush(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
codec::CodecValue cmdRPush(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
codec::CodecValue cmdLPop(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
codec::CodecValue cmdRPop(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
codec::CodecValue cmdLRange(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
}