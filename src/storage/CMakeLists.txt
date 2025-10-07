add_library(Storage
    KeyValueStore.cpp
    KeyValueStore.h
    StorageTypes.h
)

target_include_directories(Storage PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)