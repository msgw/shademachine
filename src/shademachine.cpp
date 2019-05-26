#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <iterator>
#include <functional>

#include "shademachine.h"

#include <shaderc/shaderc.hpp>

#include <spirv_glsl.hpp>
#include <spirv_msl.hpp>
#include <spirv_hlsl.hpp>
#include <spirv_cpp.hpp>

#include "filepath.h"

using namespace std;

// boilerplate filestream read
string ReadShader(string file)
{
    ifstream shaderFileStream;
    
    shaderFileStream.open(file);
    
    if(!shaderFileStream)
    {
        cerr << "Failed to open shader \"" << file << "\"" << endl;
        return string();
    }
    
    string shaderSource;
    
    string line;
    while ( getline (shaderFileStream,line) )
    {
        if(!shaderSource.empty())
        {
            shaderSource.append("\n");
        }
        shaderSource.append(line);
    }
    shaderFileStream.close();
    
    return shaderSource;
}

class Includer
: public shaderc::CompileOptions::IncluderInterface
{
public:
    Includer(string rootDir, list<string> includeDirs)
    : mRootDir(rootDir)
    , mIncludeDirs(includeDirs)
    {
        
    }
    
    virtual shaderc_include_result* GetInclude(const char* requested_source,
                                               shaderc_include_type type,
                                               const char* requesting_source,
                                               size_t include_depth)
    {
        shaderc_include_result* result = new shaderc_include_result;
        
        string shaderFile;
        string shaderSource;
        
        if(type == shaderc_include_type_relative)
        {
            shaderFile = mRootDir + requested_source;
            shaderSource = ReadShader(shaderFile);
        }
        else
        {
            for(auto includeDir : mIncludeDirs)
            {
                shaderFile = filepath::get_absolute_path(includeDir + "/" + requested_source);
                shaderSource = ReadShader(shaderFile);
                
                if(!shaderSource.empty())
                {
                    break;
                }
            }
        }
        
        if(shaderSource.empty())
        {
            char* source_name = new char[1];
            source_name[0] = '\0';
            
            char* content = new char[2048];
            snprintf(content, 2048, "Include file %s was not found.", requested_source);
            
            result->source_name = source_name;
            result->source_name_length = 0;
            result->content = content;
            result->content_length = strlen(content);
        }
        else
        {
            char* source_name = new char[shaderFile.length()+1];
            char* content = new char[shaderSource.length()+1];
            strncpy(source_name, shaderFile.c_str(), shaderFile.length()+1);
            strncpy(content, shaderSource.c_str(), shaderSource.length()+1);
            
            result->source_name = source_name;
            result->source_name_length = shaderFile.length();
            result->content = content;
            result->content_length = shaderSource.length();
        }
        
        return result;
    }
    
    // Handles shaderc_include_result_release_fn callbacks.
    virtual void ReleaseInclude(shaderc_include_result* data)
    {
        delete[] data->content;
        delete[] data->source_name;
        delete data;
    }
    
    string mRootDir;
    list<string> mIncludeDirs;
};

size_t ShadeMachine::CompileSource(const string& source, const string& name, uint8_t type, vector<uint32_t>& spirv, bool hlsl)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    if(hlsl)
    {
        options.SetSourceLanguage(shaderc_source_language_hlsl);
    }
    else
    {
        options.SetSourceLanguage(shaderc_source_language_glsl);
    }
    
    shaderc_shader_kind shader_kind;
    
    switch(type)
    {
        case Compute:
            shader_kind = shaderc_compute_shader;
            break;
        case Vertex:
            shader_kind = shaderc_vertex_shader;
            break;
        case Fragment:
            shader_kind = shaderc_fragment_shader;
            break;
    }
    
    switch(mOptimizer)
    {
        case Disable:
            options.SetOptimizationLevel(shaderc_optimization_level_zero);
            break;
        case Size:
            options.SetOptimizationLevel(shaderc_optimization_level_size);
            break;
        case Speed:
            options.SetOptimizationLevel(shaderc_optimization_level_performance);
            break;
    }
    
    options.SetIncluder(make_unique<Includer>(mShaderRootPath, mIncludeDirectories));
    
    shaderc::CompilationResult<uint32_t> result = compiler.CompileGlslToSpv(source,
                                                                            shader_kind,
                                                                            name.c_str(),
                                                                            options);
    
    if(result.GetNumErrors() > 0)
    {
        cerr << result.GetErrorMessage();
        return result.GetNumErrors();
    }
    
    copy(result.begin(), result.end(), back_inserter(spirv));
    
    return 0;
}

size_t WriteFile(string path, string name, string ext, string contents)
{
    // create the root output directory (since fstream::open won't do it for us)
    filepath::make_directory(path.c_str());
    
    string filename = path + "/" + name + "." + ext;
    ofstream filestream;
    
    filestream.open(filename);
    if(!filestream)
    {
        cerr << "Could not open output file \"" << filename << "\"" << endl;
        return 1;
    }
    
    filestream << contents;
    
    filestream.close();
    
    return 0;
}

size_t ShadeMachine::EmitGlsl32(const vector<uint32_t>& spirv, const string& outputPath, const string& name)
{
    spirv_cross::CompilerGLSL glsl(move(spirv));
    
    spirv_cross::CompilerGLSL::Options options;
    options.version = 410;
    options.es = false;
    options.enable_420pack_extension = false;
    glsl.set_common_options(options);
    
    string source = glsl.compile();
    
    return WriteFile(outputPath, name, "glsl", source);
}

size_t ShadeMachine::EmitEssl32(const vector<uint32_t>& spirv, const string& outputPath, const string& name)
{
    spirv_cross::CompilerGLSL glsl(move(spirv));
    
    spirv_cross::CompilerGLSL::Options options;
    options.version = 320;
    options.es = true;
    glsl.set_common_options(options);
    
    string source = glsl.compile();
    
    return WriteFile(outputPath, name, "glsl", source);
}

size_t ShadeMachine::EmitEssl30(const vector<uint32_t>& spirv, const string& outputPath, const string& name)
{
    spirv_cross::CompilerGLSL glsl(move(spirv));
    
    spirv_cross::CompilerGLSL::Options options;
    options.version = 300;
    options.es = true;
    glsl.set_common_options(options);
    
    string source = glsl.compile();
    
    return WriteFile(outputPath, name, "glsl", source);
}

size_t ShadeMachine::EmitMetal(const vector<uint32_t>& spirv, const string& outputPath, const string& name)
{
    spirv_cross::CompilerMSL msl(move(spirv));
    
    string source = msl.compile();
    
    return WriteFile(outputPath, name, "metal", source);
}

size_t ShadeMachine::EmitVulkan(const vector<uint32_t>& spirv, const string& outputPath, const string& name)
{
    spirv_cross::CompilerGLSL glsl(move(spirv));
    
    spirv_cross::CompilerGLSL::Options options;
    options.version = 450;
    glsl.set_common_options(options);
    
    string source = glsl.compile();
    
    return WriteFile(outputPath, name, "vk", source);
}

size_t ShadeMachine::EmitHlsl(const vector<uint32_t>& spirv, const string& outputPath, const string& name)
{
    spirv_cross::CompilerHLSL hlsl(move(spirv));
    
    spirv_cross::CompilerHLSL::Options options;
    options.shader_model = 60;
    hlsl.set_hlsl_options(options);
    
    string source = hlsl.compile();
    
    return WriteFile(outputPath, name, "hlsl", source);
}

size_t ShadeMachine::EmitCpp(const vector<uint32_t>& spirv, const string& outputPath, const string& name)
{
    spirv_cross::CompilerCPP cpp(move(spirv));
    
    string source = cpp.compile();
    
    return WriteFile(outputPath, name, "cpp", source);
}

void AppendResource(string type, spirv_cross::SmallVector<spirv_cross::Resource> resources, string& output)
{
    for(auto resource : resources)
    {
        output += type + ": " + resource.name + '\n';
    }
}

size_t ShadeMachine::EmitReflection(const vector<uint32_t>& spirv, const string& outputPath, const string& name)
{
    spirv_cross::Compiler ref(move(spirv));
    
    spirv_cross::ShaderResources reflection = ref.get_shader_resources();
    
    string ref_source;
    
    AppendResource("sampler", reflection.sampled_images, ref_source);
    AppendResource("uniform_block", reflection.uniform_buffers, ref_source);
    
    return WriteFile(outputPath, name, "ref", ref_source);
}

void ShadeMachine::SetOptimizer(Optimizer mode)
{
    mOptimizer = mode;
}

void ShadeMachine::SetIncludePaths(list<string> includeDirectories)
{
    mIncludeDirectories = includeDirectories;
}

void ShadeMachine::SetOutputPath(string shaderOutputPath)
{
    mShaderOutputPath = shaderOutputPath;
}

ShadeMachine::Error ShadeMachine::ProcessSource(string shaderFile, Shader type)
{
    string name = filepath::get_absolute_path(shaderFile);
    mShaderRootPath = name;
    
    bool hlsl = false;
    
    if(mShaderOutputPath.empty())
    {
        return OutputDirectoryNotSpecified;
    }
    
    // Remove directory from source file path.
    const size_t lastseparator = name.find_last_of("\\/");
    if (string::npos != lastseparator)
    {
        name.erase(0, lastseparator + 1);
        mShaderRootPath.erase(lastseparator + 1, shaderFile.length()+1);
    }
    
    // create the root output directory (since fstream::open won't do it for us)
    filepath::make_directory(mShaderOutputPath.c_str());
    
    string ext;
    
    // Remove extension if present.
    const size_t dot = name.rfind('.');
    if (string::npos != dot)
    {
        ext.append(name.c_str()+dot+1);
        name.erase(dot);
    }
    
    // maybe I should try to do language detection by inspection?
    string upper_ext;
    for (auto & c: ext) upper_ext += toupper(c);
    
    if(upper_ext.compare("HLSL") == 0)
    {
        hlsl = true;
    }
    
    string shaderSource = ReadShader(shaderFile);
    if(shaderSource.empty())
    {
        return SourceFileNotFound;
    }
    
    vector<uint32_t> spirv;
    
    // compile source
    int errors = (int)CompileSource(shaderSource, name, type, spirv, hlsl);
    
    if(errors > 0)
    {
        // didn't get a valid compile
        return Compile;
    }
    
    errors += (int)EmitReflection(spirv, mShaderOutputPath + "/ref", name);
    errors += (int)EmitGlsl32(spirv, mShaderOutputPath + "/GL_3_2", name);
    errors += (int)EmitEssl32(spirv, mShaderOutputPath + "/GLES_3_2", name);
    // Compute shaders not supported by GLES 3.0
    if(type != Compute)
    {
        errors += (int)EmitEssl30(spirv, mShaderOutputPath + "/GLES_3_0", name);
    }
    errors += (int)EmitMetal(spirv, mShaderOutputPath + "/metal", name);
    errors += (int)EmitVulkan(spirv, mShaderOutputPath + "/vulkan", name);
    errors += (int)EmitHlsl(spirv, mShaderOutputPath + "/hlsl", name);
    errors += (int)EmitCpp(spirv, mShaderOutputPath + "/cpp", name);
    
    return errors > 0 ? Decompile : None;
}
