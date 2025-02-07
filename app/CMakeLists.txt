add_executable(spherized Sphereized.cpp)
target_link_libraries(spherized ${PROJECT_NAME} pthread)

add_executable(convex Convex.cpp)
target_link_libraries(convex ${PROJECT_NAME} pthread)