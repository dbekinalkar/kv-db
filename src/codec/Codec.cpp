#include "Codec.h"
#include "Decode.h"
#include "Encode.h"
#include <map>
#include <sstream>

namespace codec {

std::string Codec::encode(const CodecValue& value)
{
    return std::visit(EncoderVisitor {}, value.data);
}

CodecValue Codec::decode(const std::string& data)
{
    size_t pos = 0;
    return decodeValue(data, pos);
}

} // namespace codec
