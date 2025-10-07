add_library(Codec
    Codec.cpp
    Codec.h
    CodecValue.h
    Decode.cpp
    Decode.h
    Encode.h
    CodecValue.cpp
    Marker.h
)

target_include_directories(Codec PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
