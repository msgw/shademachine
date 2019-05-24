option(BUILD_TESTING "Set to ON to build tests" OFF)

option(CXXOPTS_BUILD_EXAMPLES "Set to ON to build examples" OFF)
option(CXXOPTS_BUILD_TESTS "Set to ON to build tests" OFF)

add_subdirectory(cxxopts)

option(SPIRV_SKIP_TESTS "Set to ON to build tests" OFF)

add_subdirectory(SPIRV-Headers)

add_subdirectory(SPIRV-Tools)

add_subdirectory(glslang)

option(SPIRV_CROSS_ENABLE_TESTS "Enable SPIRV-Cross tests." OFF)

add_subdirectory(SPIRV-Cross)

option(SHADERC_SKIP_TESTS "Set to OFF to build tests" ON)

add_subdirectory(shaderc)

