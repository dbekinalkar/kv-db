#pragma once

#include "Codec.h"
#include "KeyValueStore.h"

namespace command {
codec::CodecValue cmdHSet(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
codec::CodecValue cmdHGet(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
codec::CodecValue cmdHDel(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
codec::CodecValue cmdHGetAll(storage::KeyValueStore& store, const std::vector<codec::CodecValue>& args);
}