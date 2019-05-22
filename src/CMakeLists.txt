set(SHADEMACHINE_SOURCES
    main.cpp
    filesystem.h
    filesystem.cpp
    shademachine.h
    shademachine.cpp
)

add_executable(shademachine ${SHADEMACHINE_SOURCES})

target_link_libraries(shademachine
    cxxopts
    shaderc
    spirv-cross-core
    spirv-cross-glsl
    spirv-cross-cpp
    spirv-cross-reflect
    spirv-cross-msl
    spirv-cross-hlsl
    spirv-cross-util
)
