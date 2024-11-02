set(BOT_LIB_NAME SphereTree)
file(GLOB_RECURSE LibSource ./*.cpp)
add_library(${BOT_LIB_NAME} ${COMPILE_TYPE}
        ${LibSource}
)
set_target_properties(${BOT_LIB_NAME} PROPERTIES VERSION "${${PROJECT_NAME}_VERSION}")

target_link_libraries(${BOT_LIB_NAME} Geometry Storage)
set_target_properties(${BOT_LIB_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_SO_VERSION})
