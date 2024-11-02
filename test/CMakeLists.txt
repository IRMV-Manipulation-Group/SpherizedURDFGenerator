find_package(GTest REQUIRED)
find_library(GMP_LIBRARY NAMES gmp)

add_executable(test_manifoldplus test_manifoldplus.cpp)

# Link the test executable to Google Test and pthread library
target_include_directories(test_manifoldplus PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(test_manifoldplus PRIVATE GTest::GTest GTest::Main pthread ManifoldPlus  ${Boost_LIBRARIES})


add_executable(test_simplify test_simplify.cpp)
target_link_libraries(test_simplify PRIVATE GTest::GTest GTest::Main pthread  ${Boost_LIBRARIES} ${GMP_LIBRARY})

add_executable(test_spheretree test_spheretree.cpp)
target_link_libraries(test_spheretree PRIVATE GTest::GTest GTest::Main pthread ${Boost_LIBRARIES} sphereTreeWrapper )

add_executable(test_urdf_genderator test_urdf_genderator.cpp)
target_link_libraries(test_urdf_genderator PRIVATE GTest::GTest GTest::Main pthread ${PROJECT_NAME} ${Boost_LIBRARIES} sphereTreeWrapper )


include(GoogleTest)
gtest_discover_tests(test_manifoldplus test_simplify test_spheretree test_urdf_genderator )