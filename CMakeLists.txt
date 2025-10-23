cmake_minimum_required(VERSION 3.10)
project(OriAssistant)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
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
    src/plugins/orpm.cpp
)

# Create executable
add_executable(ori ${SOURCES})

# Find required packages
find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(JSONCPP jsoncpp)
    pkg_check_modules(CURL libcurl)
endif()

# Include directories
if(JSONCPP_FOUND)
    include_directories(${JSONCPP_INCLUDE_DIRS})
    add_definitions(-DJSONCPP_FOUND)
endif()

if(CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DCURL_FOUND)
endif()

# Link libraries
if(JSONCPP_FOUND)
    target_link_libraries(ori ${JSONCPP_LIBRARIES})
else()
    message(WARNING "jsoncpp not found, plugin database loading may not work")
endif()

if(CURL_FOUND)
    target_link_libraries(ori ${CURL_LIBRARIES})
else()
    message(WARNING "libcurl not found, API calls will not work")
endif()

# Additional libraries that might be needed
# find_package(CURL)
# if(CURL_FOUND)
#     target_link_libraries(ori ${CURL_LIBRARIES})
# endif()

# Copy system prompt into build directory


# Install targets
install(TARGETS ori DESTINATION bin)
install(DIRECTORY plugins/ DESTINATION share/Ori/plugins OPTIONAL)
