#pragma once

#include <vector>

class ShadeMachine
{
public:
    
    enum Error
    {
        None,
        SourceFileNotSpecified,
        OutputDirectoryNotSpecified,
        SourceFileNotFound,
        CouldNotCreateOutputFile,
        Compile,
        Decompile
    };
    
    enum Shader
    {
        Vertex,
        Fragment,
        Compute
    };
    
    enum Optimizer
    {
        Speed,
        Size,
        Disable
    };
    
    void SetOptimizer(Optimizer mode = Disable);
    
    void SetIncludePaths(std::list<std::string> includes);
    
    void SetOutputPath(std::string shaderOutputPath);
    
    Error ProcessSource(std::string shaderFile, Shader type);
    
private:
    
    size_t EmitMetal(const std::vector<uint32_t>& spirv, const std::string& outputPath, const std::string& name);
    
    size_t EmitVulkan(const std::vector<uint32_t>& spirv, const std::string& outputPath, const std::string& name);
    
    size_t EmitHlsl(const std::vector<uint32_t>& spirv, const std::string& outputPath, const std::string& name);
    
    size_t EmitGlsl32(const std::vector<uint32_t>& spirv, const std::string& outputPath, const std::string& name);

    size_t EmitEssl32(const std::vector<uint32_t>& spirv, const std::string& outputPath, const std::string& name);
    
    size_t EmitEssl30(const std::vector<uint32_t>& spirv, const std::string& outputPath, const std::string& name);
    
    size_t EmitCpp(const std::vector<uint32_t>& spirv, const std::string& outputPath, const std::string& name);
    
    size_t EmitReflection(const std::vector<uint32_t>& spirv, const std::string& outputPath, const std::string& name);
    
    size_t CompileSource(const std::string& source, const std::string& name, uint8_t type, std::vector<uint32_t>& spirv, bool hlsl);
    
    std::string mShaderOutputPath;
    std::string mShaderRootPath;
    
    std::list<std::string> mIncludeDirectories;
    
    Optimizer mOptimizer;
};
