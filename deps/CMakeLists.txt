option(BUILD_TESTING "Set to ON to build tests" OFF)

option(CXXOPTS_BUILD_EXAMPLES "Set to ON to build examples" OFF)
option(CXXOPTS_BUILD_TESTS "Set to ON to build tests" OFF)

add_subdirectory(cxxopts)

option(SPIRV_SKIP_EXECUTABLES "Set to OFF to build executables" OFF)
option(SPIRV_SKIP_TESTS "Set to OFF to build tests" ON)
option(SPIRV_CROSS_ENABLE_TESTS "Enable SPIRV-Cross tests." OFF)
option(SHADERC_ENABLE_SHARED_CRT "Use Shared CRT" OFF)
option(SHADERC_SKIP_TESTS "Set to OFF to build tests" ON)

#add_subdirectory(spirv-headers)

#add_subdirectory(spirv-tools)

#add_subdirectory(glslang)

set(SHADERC_THIRD_PARTY_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE STRING "ShaderC deps path")

add_subdirectory(shaderc)

add_subdirectory(spirv-cross)

