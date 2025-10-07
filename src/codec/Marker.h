#pragma once

#include "CodecValue.h"

const inline std::string DELIMITER = "\r\n";

using MarkerType = char;
const MarkerType MARKER_SIMPLE_STRING = '+';
const MarkerType MARKER_ERROR = '-';
const MarkerType MARKER_INTEGER = ':';
const MarkerType MARKER_BULK_STRING = '$';
const MarkerType MARKER_ARRAY = '*';
