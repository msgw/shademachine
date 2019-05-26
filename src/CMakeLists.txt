set(SHADEMACHINE_SOURCES
    main.cpp
    filepath.h
    filepath.cpp
    shademachine.h
    shademachine.cpp
)

if(WIN32)
    add_executable(shademachine ${SHADEMACHINE_SOURCES})
else()
    add_executable(shademachine ${SHADEMACHINE_SOURCES})
endif()

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
