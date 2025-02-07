# find a substring from a string by a given prefix such as VCVARSALL_ENV_START
function(
  find_substring_by_prefix
  output
  prefix
  input)
  # find the prefix
  string(FIND "${input}" "${prefix}" prefix_index)
  if("${prefix_index}" STREQUAL "-1")
    message(SEND_ERROR "Could not find ${prefix} in ${input}")
  endif()
  # find the start index
  string(LENGTH "${prefix}" prefix_length)
  math(EXPR start_index "${prefix_index} + ${prefix_length}")

  string(
    SUBSTRING "${input}"
              "${start_index}"
              "-1"
              _output)
  set("${output}"
      "${_output}"
      PARENT_SCOPE)
endfunction()

# A function to set environment variables of CMake from the output of `cmd /c set`
function(set_env_from_string env_string)
  # replace ; in paths with __sep__ so we can split on ;
  string(
    REGEX
    REPLACE ";"
            "__sep__"
            env_string_sep_added
            "${env_string}")

  # the variables are separated by \r?\n
  string(
    REGEX
    REPLACE "\r?\n"
            ";"
            env_list
            "${env_string_sep_added}")

  foreach(env_var ${env_list})
    # split by =
    string(
      REGEX
      REPLACE "="
              ";"
              env_parts
              "${env_var}")

    list(LENGTH env_parts env_parts_length)
    if("${env_parts_length}" EQUAL "2")
      # get the variable name and value
      list(
        GET
        env_parts
        0
        env_name)
      list(
        GET
        env_parts
        1
        env_value)

      # recover ; in paths
      string(
        REGEX
        REPLACE "__sep__"
                ";"
                env_value
                "${env_value}")

      # set env_name to env_value
      set(ENV{${env_name}} "${env_value}")

      # update cmake program path
      if("${env_name}" EQUAL "PATH")
        list(APPEND CMAKE_PROGRAM_PATH ${env_value})
      endif()
    endif()
  endforeach()
endfunction()

function(get_all_targets var)
  set(targets)
  get_all_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
  set(${var}
      ${targets}
      PARENT_SCOPE)
endfunction()

function(get_all_installable_targets var)
  set(targets)
  get_all_targets(targets)
  foreach(_target ${targets})
    get_target_property(_target_type ${_target} TYPE)
    if(NOT
       ${_target_type}
       MATCHES
       ".*LIBRARY|EXECUTABLE")
      list(REMOVE_ITEM targets ${_target})
    endif()
  endforeach()
  set(${var}
      ${targets}
      PARENT_SCOPE)
endfunction()

macro(get_all_targets_recursive targets dir)
  get_property(
    subdirectories
    DIRECTORY ${dir}
    PROPERTY SUBDIRECTORIES)
  foreach(subdir ${subdirectories})
    get_all_targets_recursive(${targets} ${subdir})
  endforeach()

  get_property(
    current_targets
    DIRECTORY ${dir}
    PROPERTY BUILDSYSTEM_TARGETS)
  list(APPEND ${targets} ${current_targets})
endmacro()

function(is_verbose var)
  if("CMAKE_MESSAGE_LOG_LEVEL" STREQUAL "VERBOSE"
     OR "CMAKE_MESSAGE_LOG_LEVEL" STREQUAL "DEBUG"
     OR "CMAKE_MESSAGE_LOG_LEVEL" STREQUAL "TRACE")
    set(${var}
        ON
        PARENT_SCOPE)
  else()
    set(${var}
        OFF
        PARENT_SCOPE)
  endif()
endfunction()

# Function to format dependencies with "lib" prefix and version requirements
function(prepare_debian_dependencies dependencies output_var)
  # Check if the input dependencies list is empty
  if ("${dependencies}" STREQUAL "")
    set(${output_var} "" PARENT_SCOPE)
    return()
  endif()

  set(result "")

  foreach(dep ${dependencies})
    string(REPLACE " " ";" dep_parts ${dep})  # Split name and version
    list(GET dep_parts 0 PKG_NAME)
    list(GET dep_parts 1 PKG_VERSION)

    # Add "lib" prefix, format with version
    set(formatted_dep "${PKG_NAME} (>= ${PKG_VERSION})")

    # Append to result list
    if(result)
      set(result "${result}, ${formatted_dep}")
    else()
      set(result "${formatted_dep}")
    endif()
  endforeach()

  # Output to specified variable
  set(${output_var} "${result}" PARENT_SCOPE)
endfunction()

function(prepare_irmv_dependencies dependencies output_var)
  # Check if the input dependencies list is empty
  if ("${dependencies}" STREQUAL "")
    set(${output_var} "" PARENT_SCOPE)
    return()
  endif()

  set(result "")

  foreach(dep ${dependencies})
    string(REPLACE " " ";" dep_parts ${dep})  # Split name and version
    list(GET dep_parts 0 PKG_NAME)
    list(GET dep_parts 1 PKG_VERSION)
    set(formatted_dep "${PKG_NAME} (>= ${PKG_VERSION})")

    find_package(${PKG_NAME} ${PKG_VERSION} QUIET)
    get_property(GLOBAL_${PKG_NAME}_LIBRARIES GLOBAL PROPERTY ${PKG_NAME}_LIBRARIES)
    if (NOT ${PKG_NAME}_FOUND AND "${GLOBAL_${PKG_NAME}_LIBRARIES}" STREQUAL "")
      message(WARNING "Please install ${PKG_NAME} ${PKG_VERSION} first, try to include source code")
      add_subdirectory(${PKG_NAME})
      set(${PKG_NAME}_LIBRARIES ${PKG_NAME} PARENT_SCOPE)
      set(${PKG_NAME}_INCLUDE_DIRS "${${PKG_NAME}_INCLUDE_DIRS}" PARENT_SCOPE)

      set_property(GLOBAL PROPERTY ${PKG_NAME}_LIBRARIES ${PKG_NAME})
      set_property(GLOBAL PROPERTY ${PKG_NAME}_INCLUDE_DIRS "${${PKG_NAME}_INCLUDE_DIRS}")
    elseif (${PKG_NAME}_FOUND)
      set(${PKG_NAME}_LIBRARIES ${PKG_NAME}::${PKG_NAME} PARENT_SCOPE)
    endif ()
    # Add "lib" prefix, format with version
    set(formatted_dep "${PKG_NAME} (>= ${PKG_VERSION})")

    # Append to result list
    if(result)
      set(result "${result}, ${formatted_dep}")
    else()
      set(result "${formatted_dep}")
    endif()
  endforeach()

  # Output to specified variable
  set(${output_var} "${result}" PARENT_SCOPE)
endfunction()

function(add_gtest_discover_for_file test_file)
  # Parse optional LINK_LIBRARIES argument to accept multiple libraries
  cmake_parse_arguments(ARG "" "" "LINK_LIBRARIES" ${ARGN})

  # Extract the filename without extension to use as the test target name
  get_filename_component(test_name ${test_file} NAME_WE)

  # Create an executable for the test
  add_executable(${test_name} ${test_file})

  # Link the executable to GTest, pthread, and any additional libraries
  target_link_libraries(${test_name} PRIVATE GTest::GTest GTest::Main pthread ${ARG_LINK_LIBRARIES})

  # Add the test to the test suite
  gtest_discover_tests(${test_name})
endfunction()


function(add_benchmark_discover_for_file bench_file)
  # Parse optional LINK_LIBRARIES argument to accept multiple libraries
  cmake_parse_arguments(ARG "" "" "LINK_LIBRARIES" ${ARGN})

  # Extract the filename without extension to use as the test target name
  get_filename_component(bench_name ${bench_file} NAME_WE)

  # Create an executable for the test
  add_executable(${bench_name} ${bench_file})

  # Link the executable to GTest, pthread, and any additional libraries
  target_link_libraries(${bench_name} PRIVATE benchmark::benchmark pthread ${ARG_LINK_LIBRARIES})

endfunction()