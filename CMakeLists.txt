cmake_minimum_required(VERSION 3.12)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

project(EnforceExtended)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE sources
        "src/*.cpp"
        "include/*.h")

add_library(EnforceExtended SHARED ${sources})

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (WIN32)
    target_compile_definitions(EnforceExtended PRIVATE WIN32_LEAN_AND_MEAN)
    target_link_libraries(EnforceExtended PRIVATE ws2_32)
endif (WIN32)

target_include_directories(EnforceExtended PRIVATE ${CMAKE_SOURCE_DIR}/include)

#rcmp
add_subdirectory(vendor/rcmp)
target_link_libraries(EnforceExtended PRIVATE rcmp)

#MinHook
#add_subdirectory(vendor/minhook)
#target_link_libraries(EnforceExtended PRIVATE minhook)

if (WIN32)
    set(
            ARCHIVE_OUTPUT_DIRECTORY "C:/Program Files (x86)/Steam/steamapps/common/DayZServer/"
            LIBRARY_OUTPUT_DIRECTORY "C:/Program Files (x86)/Steam/steamapps/common/DayZServer/"
            RUNTIME_OUTPUT_DIRECTORY "C:/Program Files (x86)/Steam/steamapps/common/DayZServer/"
    )

    set_target_properties(EnforceExtended PROPERTIES RUNTIME_OUTPUT_DIRECTORY "C:/Program Files (x86)/Steam/steamapps/common/DayZServer/")
endif (WIN32)