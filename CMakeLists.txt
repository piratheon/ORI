cmake_minimum_required(VERSION 3.10)
project(OriAssistant)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define installation data directory
set(ORI_DATA_DIR "/usr/share/ori")
add_definitions(-DORI_DATA_DIR="${ORI_DATA_DIR}")

# Add include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/core/ori_core.cpp
    src/core/ori_edit.cpp
    src/plugins/orpm.cpp
)

# Create executable
add_executable(ori ${SOURCES})

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)
pkg_check_modules(CURL REQUIRED libcurl)

# Include directories and compile definitions
if(JSONCPP_FOUND)
    target_include_directories(ori PRIVATE ${JSONCPP_INCLUDE_DIRS})
    target_compile_definitions(ori PRIVATE JSONCPP_FOUND)
else()
    message(FATAL_ERROR "jsoncpp is required to build ORI")
endif()

if(CURL_FOUND)
    target_include_directories(ori PRIVATE ${CURL_INCLUDE_DIRS})
    target_compile_definitions(ori PRIVATE CURL_FOUND)
else()
    message(FATAL_ERROR "libcurl is required to build ORI")
endif()

# Link libraries
target_link_libraries(ori PRIVATE ${JSONCPP_LIBRARIES} ${CURL_LIBRARIES} stdc++fs)

# Additional libraries that might be needed
# find_package(CURL)
# if(CURL_FOUND)
#     target_link_libraries(ori ${CURL_LIBRARIES})
# endif()

# Copy system prompt into build directory


# Install targets
install(TARGETS ori DESTINATION bin)
install(FILES orpm DESTINATION bin)

# Install additional data files for packaging
install(DIRECTORY plugins/ DESTINATION share/Ori/plugins OPTIONAL)
install(FILES LICENSE DESTINATION share/Ori)
install(DIRECTORY include/ DESTINATION include/ori OPTIONAL)

# Provide a CMake install config (optional simple staging)
include(GNUInstallDirs)
