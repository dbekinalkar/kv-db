#pragma once

#include "Codec.h"
#include "KeyValueStore.h"

namespace command {

class CommandProcessor {
public:
    CommandProcessor(storage::KeyValueStore& kvStore);

    codec::CodecValue process(const codec::CodecValue& msg) const;

private:
    storage::KeyValueStore& kvStore_;
};

} // namespace command