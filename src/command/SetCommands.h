#pragma once

#include "Codec.h"
#include "KeyValueStore.h"

namespace command {
codec::CodecValue cmdSAdd(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
codec::CodecValue cmdSRem(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
codec::CodecValue cmdSMembers(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
}