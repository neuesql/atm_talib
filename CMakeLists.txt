cmake_minimum_required(VERSION 3.5)

set(TARGET_NAME talib)
set(EXTENSION_NAME ${TARGET_NAME}_extension)
set(LOADABLE_EXTENSION_NAME ${TARGET_NAME}_loadable_extension)

project(${TARGET_NAME})

include(FetchContent)
FetchContent_Declare(
    ta_lib
    GIT_REPOSITORY https://github.com/ta-lib/ta-lib.git
    GIT_TAG v0.6.4
)
FetchContent_MakeAvailable(ta_lib)

include_directories(src/include)
include_directories(${ta_lib_SOURCE_DIR}/include)

set(EXTENSION_SOURCES
    src/talib_extension.cpp
    src/talib_adapter.cpp
    src/talib_scalar.cpp
    src/talib_aggregate.cpp
    src/talib_multi_output.cpp
    src/talib_multi_output_agg.cpp
)

build_static_extension(${TARGET_NAME} ${EXTENSION_SOURCES})
build_loadable_extension(${TARGET_NAME} " " ${EXTENSION_SOURCES})

# Link TA-Lib static library via archive file to avoid CMake export set issues
target_link_libraries(${EXTENSION_NAME} $<TARGET_FILE:ta-lib-static>)
target_link_libraries(${LOADABLE_EXTENSION_NAME} $<TARGET_FILE:ta-lib-static>)

# Ensure ta-lib-static is built before our extension
add_dependencies(${EXTENSION_NAME} ta-lib-static)
add_dependencies(${LOADABLE_EXTENSION_NAME} ta-lib-static)

install(
    TARGETS ${EXTENSION_NAME} ${LOADABLE_EXTENSION_NAME}
    EXPORT "${DUCKDB_EXPORT_SET}"
    LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
)
