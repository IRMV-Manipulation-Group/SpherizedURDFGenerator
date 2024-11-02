set(BOT_LIB_NAME Exceptions)
file(GLOB_RECURSE LibSource ./*.cpp)
add_library(${BOT_LIB_NAME} ${COMPILE_TYPE}
        ErrorLog.cpp
        ErrorLog.h
        MemoryException.cpp
        MemoryException.h
        Exception.cpp
        Exception.h
        CheckAlways.h
        CheckDebug.h
        CheckMemory.h
        ValidPointer.h
        ErrorsInc.h
)
set(EXTRA_DIST
        CallStack.cpp
        CallStack.h
)
set_target_properties(${BOT_LIB_NAME} PROPERTIES VERSION "${${PROJECT_NAME}_VERSION}")

target_link_libraries(${BOT_LIB_NAME})
set_target_properties(${BOT_LIB_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_SO_VERSION})
