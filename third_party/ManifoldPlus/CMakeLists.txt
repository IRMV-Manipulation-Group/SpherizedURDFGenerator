# Notice: This ManifoldPlus library is originally presented by Jingwei Huang. Non-commercial usage only
# Detailed information can be found at https://github.com/hjwdzh/ManifoldPlus
set(BOT_LIB_NAME ManifoldPlus)
file(GLOB_RECURSE LibSource src/*.cc)
add_library(${BOT_LIB_NAME} ${COMPILE_TYPE}
        ${LibSource}
)
set_target_properties(${BOT_LIB_NAME} PROPERTIES VERSION "${${PROJECT_NAME}_VERSION}")

target_link_libraries(${BOT_LIB_NAME})
set_target_properties(${BOT_LIB_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_SO_VERSION})

install(TARGETS ${BOT_LIB_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(DIRECTORY include/${BOT_LIB_NAME}/ DESTINATION include/${BOT_LIB_NAME}
        FILES_MATCHING PATTERN "*.h"
)

option(BUILD_MANIFOLD "Enable executable file build for ManifoldPlus" ON)
if(BUILD_MANIFOLD)
    add_executable( manifold
            main.cc
    )

    target_link_libraries(manifold ${BOT_LIB_NAME} pthread)
endif(BUILD_MANIFOLD)