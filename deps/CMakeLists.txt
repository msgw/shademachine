include(FetchContent)

set(Python_ADDITIONAL_VERSIONS 3.6)

# For Buildable Cmake projects
macro(Fetch name URL)
    message("Fetching: ${name} - ${URL}")
    FetchContent_Declare(
        ${name}
        GIT_REPOSITORY ${URL}
    )

    FetchContent_GetProperties(${name})
    if(NOT ${name}_POPULATED)
      FetchContent_Populate(${name})
      add_subdirectory(${${name}_SOURCE_DIR} ${${name}_BINARY_DIR})
    endif()
endmacro()

# For Header Only
macro(FetchNoBuild name URL)
    FetchContent_Declare(
        ${name}
        GIT_REPOSITORY ${URL}
    )

    FetchContent_GetProperties(${name})
    if(NOT ${name}_POPULATED)
        FetchContent_Populate(${name})
        add_library(${name} INTERFACE)
        target_include_directories(
            ${name}
            INTERFACE
            $<BUILD_INTERFACE:${${name}_SOURCE_DIR}>
        )
    endif()
endmacro()

option(BUILD_TESTING "Set to ON to build tests" OFF)

Fetch(catch "git@gitlab.com:msgw/external/Catch2.git")

option(CXXOPTS_BUILD_EXAMPLES "Set to ON to build examples" OFF)
option(CXXOPTS_BUILD_TESTS "Set to ON to build tests" OFF)

Fetch(cxxopts "git@gitlab.com:msgw/external/cxxopts.git")

option(SPIRV_SKIP_TESTS "Set to ON to build tests" OFF)

Fetch(spirv-headers "git@gitlab.com:msgw/external/SPIRV-Headers.git")

Fetch(spirv-tools "git@gitlab.com:msgw/external/SPIRV-Tools.git")

Fetch(glslang "git@gitlab.com:msgw/external/glslang.git")

option(SPIRV_CROSS_ENABLE_TESTS "Enable SPIRV-Cross tests." OFF)

Fetch(spirv-cross "git@gitlab.com:msgw/external/SPIRV-Cross.git")

option(SHADERC_SKIP_TESTS "Set to OFF to build tests" ON)

Fetch(shaderc "git@gitlab.com:msgw/external/shaderc.git")

