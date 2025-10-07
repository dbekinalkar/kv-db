#pragma once

#include "Codec.h"
#include "KeyValueStore.h"

namespace command {
codec::CodecValue cmdSet(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
codec::CodecValue cmdGet(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
codec::CodecValue cmdDel(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
codec::CodecValue cmdExists(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
} // namespace command