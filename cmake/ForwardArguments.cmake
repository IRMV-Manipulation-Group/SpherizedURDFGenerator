# /ForwardArguments.cmake
#
# Forward input variable-length arguments to child functions.
#
# This module provides a declarative way to collect a subset of
# variable-length arguments typically used with CMakeParseArguments
# into a single list so that they can be passed to a child function.
#
# See /LICENCE.md for Copyright information

include_guard()

include (CMakeParseArguments)

# cmake_forward_arguments
#
# Forward arguments used by cmake_parse_arguments
#
# SOURCE_PREFIX: Prefix of set variables to forward from
# RETURN_LIST: List of "forwarded" variables, suitable for passing to
#              cmake_parse_arguments
# [Optional] OPTION_ARGS: "Option" arguments (true or false)
# [Optional] SINGLEVAR_ARGS: "Single variable" arguments (variable, if
#                            set, has one value. Represented by NAME VALUE)
# [Optional] MULTIVAR_ARGS: "Multi variable" arguments (variable, if set, has
#                           a list value. Represented by NAME VALUE0 ... VALUEN)
function (cmake_forward_arguments SOURCE_PREFIX RETURN_LIST)

    cmake_parse_arguments (FORWARD
                           ""
                           ""
                           "OPTION_ARGS;SINGLEVAR_ARGS;MULTIVAR_ARGS"
                           ${ARGN})

    set (_RETURN_LIST)
    foreach (FORWARDED_OPTION ${FORWARD_OPTION_ARGS})

        if (${SOURCE_PREFIX}_${FORWARDED_OPTION})

            list (APPEND _RETURN_LIST ${FORWARDED_OPTION})

        endif ()

    endforeach ()

    foreach (FORWARDED_VAR ${FORWARD_SINGLEVAR_ARGS} ${FORWARD_MULTIVAR_ARGS})

        if (${SOURCE_PREFIX}_${FORWARDED_VAR})

            list (APPEND _RETURN_LIST
                         ${FORWARDED_VAR}
                         ${${SOURCE_PREFIX}_${FORWARDED_VAR}})

        endif ()

    endforeach ()

    set (${RETURN_LIST} ${_RETURN_LIST} PARENT_SCOPE)

endfunction ()
