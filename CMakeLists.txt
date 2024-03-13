cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(ShipTestGfx)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/port/libultra_pc ${CMAKE_BINARY_DIR}/libultra_pc)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/port/libultraship ${CMAKE_BINARY_DIR}/libultraship)

file(GLOB Sources "*.c")
list(REMOVE_ITEM Sources ${CMAKE_CURRENT_SOURCE_DIR}/test.c)
file(GLOB Headers "*.h")
file(GLOB Port_Sources "port/src/*.c" "port/src/*.cpp")
file(GLOB Port_Headers "port/src/*.h")
file(GLOB N64_Headers "port/libultra/*.h" "port/libultra/PR/*.h")

find_package(SDL2)

set(N64_INCLUDE
    ${CMAKE_CURRENT_SOURCE_DIR}/port/include
    ${CMAKE_CURRENT_SOURCE_DIR}/port/include/PR
)
set(PORT_INCLUDE
    ${SDL_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/port/libultraship/include/libultraship
    ${CMAKE_CURRENT_SOURCE_DIR}/port/libultraship/include
    ${CMAKE_CURRENT_SOURCE_DIR}/port/libultraship/src
    ${CMAKE_CURRENT_SOURCE_DIR}/port/libultraship/extern
    ${CMAKE_CURRENT_SOURCE_DIR}/port/libultraship/extern/ImGui
    ${CMAKE_CURRENT_SOURCE_DIR}/port/libultraship/extern/StormLib/src
    ${CMAKE_CURRENT_SOURCE_DIR}/port/libultraship/extern/Mercury
    ${CMAKE_CURRENT_SOURCE_DIR}/port/libultraship/extern/ZAPDUtils
    ${CMAKE_CURRENT_SOURCE_DIR}/port/libultraship/extern/tinyxml2
    ${CMAKE_CURRENT_SOURCE_DIR}/port/libultraship/extern/spdlog/include
)

set_source_files_properties(${Sources} PROPERTIES INCLUDE_DIRECTORIES "${N64_INCLUDE}")
set_source_files_properties(${Port_Sources} PROPERTIES INCLUDE_DIRECTORIES "${PORT_INCLUDE}")

set(ALL_FILES
    ${Sources}
    ${Headers}
    ${Port_Sources}
    ${Port_Headers}
    ${N64_Headers}
)

add_executable(${PROJECT_NAME} ${ALL_FILES})

add_dependencies(${PROJECT_NAME} libultraship)
add_dependencies(${PROJECT_NAME} libultra_pc)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    NDEBUG
    _FINALROM
    TARGET_PC
    SPDLOG_ACTIVE_LEVEL=0
    STORMLIB_NO_AUTO_LINK
    UNICODE
    _UNICODE
    ENABLE_OPENGL
    F3DEX_GBI_2
    $<$<COMPILE_LANGUAGE:C>:_LANGUAGE_C>
    $<$<COMPILE_LANGUAGE:CXX>:_LANGUAGE_C_PLUS_PLUS>
)

target_compile_options(${PROJECT_NAME} PRIVATE -msse2 -mfpmath=sse)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
    "libultraship;"
    "libultra_pc;"
    "ZAPDUtils;"
    SDL2::SDL2
    ${CMAKE_DL_LIBS}
    Threads::Threads
)
target_link_options(${PROJECT_NAME} PRIVATE
    -pthread
    -Wl,-export-dynamic
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "simple.elf")

add_custom_target(CopySohOtr
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/soh.otr ${CMAKE_CURRENT_BINARY_DIR}/soh.otr
    BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/soh.otr ${CMAKE_CURRENT_BINARY_DIR}/soh.otr
)
add_dependencies(${PROJECT_NAME} CopySohOtr)
