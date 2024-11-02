cmake_minimum_required(VERSION 3.10)
project(spheretree-dist VERSION 1.0 LANGUAGES CXX C)

# Include all subdirectories that are part of the project
add_subdirectory(API)
add_subdirectory(Base)
add_subdirectory(BBox)
add_subdirectory(Exceptions)
add_subdirectory(Geometry)
add_subdirectory(LinearAlgebra)
add_subdirectory(MedialAxis)
add_subdirectory(MinMax)
add_subdirectory(SphereTree)
add_subdirectory(Storage)
add_subdirectory(Surface)
add_subdirectory(Export)
# Create STG library
add_library(STG STATIC VerifyModel.cpp EvalTree.cpp)

# Include compiler warnings module
include(CompilerWarnings)
if(ENABLE_WARNINGS)
    myproject_set_project_warnings(STG OFF "" "" "" "")
endif()

# Link dependent libraries to STG
set(LIBS_TO_LINK
        API
        Base
        BBox
        Exceptions
        Geometry
        Matrix
        MedialAxis
        MinMax
        SphereTree
        Storage
        Surface
        Export
)
target_link_libraries(STG PUBLIC ${LIBS_TO_LINK})


# Enable sanitizers
if(ENABLE_SANITIZER)
    include(Sanitizers)
    add_sanitizers(STG)
endif()

# Set common properties for executables
function(add_custom_executable EXEC_NAME)
    set(SOURCES)
    foreach(_ARG IN LISTS ARGN)
        list(APPEND SOURCES ${_ARG})
    endforeach()
    add_executable(${EXEC_NAME} ${SOURCES})
    target_link_libraries(${EXEC_NAME} PRIVATE STG)
    if(ENABLE_WARNINGS)
        myproject_set_project_warnings(${EXEC_NAME} OFF "" "" "" "")
    endif()
    if(ENABLE_SANITIZER)
        add_sanitizers(${EXEC_NAME})
    endif()
endfunction()

# Add executables
add_custom_executable(makeTreeMedial
        "makeTreeMedial.cpp"
        "DecodeParam.cpp"
)

add_custom_executable(makeTreeHubbard
        "makeTreeHubbard.cpp"
        "DecodeParam.cpp"
)

add_custom_executable(makeTreeOctree
        "makeTreeOctree.cpp"
        "DecodeParam.cpp"
)

add_custom_executable(makeTreeGrid
        "makeTreeGrid.cpp"
        "DecodeParam.cpp"
)

add_custom_executable(makeTreeSpawn
        "makeTreeSpawn.cpp"
        "DecodeParam.cpp"
)


