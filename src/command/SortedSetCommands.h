#pragma once

#include "Codec.h"
#include "KeyValueStore.h"

namespace command {
codec::CodecValue cmdZAdd(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
codec::CodecValue cmdZRem(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
codec::CodecValue cmdZRange(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
}