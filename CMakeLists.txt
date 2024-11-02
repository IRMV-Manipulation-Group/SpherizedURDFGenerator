cmake_minimum_required(VERSION 3.26)
project(SpherizedURDFGenerator)
set(PROJECT_VERSION 1.1.0)
set(PROJECT_SO_VERSION 1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

option(ENABLE_WARNINGS "Enable project warnings" ON)
option(ENABLE_SANITIZER "Enable address and undefined behavior sanitizer" OFF)
option(ENABLE_TEST "Enable test " OFF)

# Set up modules path and include necessary scripts from cmake_template
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include standard project settings
include(StandardProjectSettings)

SET(COMPILE_TYPE "STATIC")


add_definitions(-DRESOURCE_PATH="${CMAKE_SOURCE_DIR}/resources")
add_definitions(-DCONFIG_PATH="${CMAKE_SOURCE_DIR}/config")

set(THIS_PACKAGE_INCLUDE_DIRS
        bot_common/include
        third_party/header_only
        third_party/ManifoldPlus/include
        third_party/spheretree
        bot_utils/sphereTreeWrapper/include
)

include_directories(
        SYSTEM
        include
        ${EIGEN3_INCLUDE_DIRS}
        ${THIS_PACKAGE_INCLUDE_DIRS}
)

find_package(urdfdom REQUIRED)

add_subdirectory(third_party/ManifoldPlus)
add_subdirectory(third_party/spheretree)
add_subdirectory(bot_utils)

add_library(${PROJECT_NAME} ${COMPILE_TYPE}
        src/SphereTreeURDFGenerator.cpp
        src/ConvexHullCollisionURDFGenerator.cpp
        src/URDFGenerator.cpp)

target_link_libraries(${PROJECT_NAME}
        gmp ManifoldPlus sphereTreeWrapper
        tinyxml2 urdfdom_model urdfdom_world urdfdom_sensor
)

if (ENABLE_TEST)
    add_subdirectory(test)
endif ()


add_executable(spherized Sphereized.cpp)
target_link_libraries(spherized ${PROJECT_NAME} pthread)

add_executable(convex Convex.cpp)
target_link_libraries(convex ${PROJECT_NAME} pthread)