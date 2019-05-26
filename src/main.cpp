#include <iostream>
#include <algorithm>
#include <string>
#include <list>
#include <regex>

#include <cxxopts.hpp>

#include "shademachine.h"
#include "filepath.h"

int main(int argc, char * argv[])
{
    // setup command line options
    cxxopts::Options options("ShadeMachine", "Shader Transpiler");
    
    // (Use [name]_[type].[lang] file naming)
    options.add_options()
        ("o", "Optimization Level", cxxopts::value<int>())
        ("I,includes", "Include Search Paths", cxxopts::value<std::string>())
        ("output", "Output Directory", cxxopts::value<std::string>());
    
    options.parse_positional("output");
    
    try
    {
        auto result = options.parse(argc, argv);
        
        // output root folder
        if(result.count("output") < 1)
        {
            std::cerr << "No output directory given" << std::endl;
            return -ShadeMachine::OutputDirectoryNotSpecified;
        }
        
        // optimization type
        ShadeMachine::Optimizer optimizer = ShadeMachine::Disable;
        
        if(result["o"].count() > 0)
        {
            switch(result["o"].as<int>())
            {
                case 1:
                    optimizer = ShadeMachine::Size;
                    break;
                case 2:
                    optimizer = ShadeMachine::Speed;
                    break;
            }
        }
        
        ShadeMachine machine;
        
        machine.SetOptimizer(optimizer);
        
        // include search paths
        std::list<std::string> include_dirs;
        if(result["I"].count() > 0)
        {
            std::string str = result["I"].as<std::string>();
            
            std::regex reg(";");
            
            std::sregex_token_iterator iter(str.begin(), str.end(), reg, -1);
            std::sregex_token_iterator end;
            
            include_dirs = std::list<std::string>(iter, end);
            
            machine.SetIncludePaths(include_dirs);
        }
        
        // list of files to transpile
		std::list<std::string> file_list = filepath::list_directory(".");// result["."].as<std::string>());
            
        for(std::string file : file_list)
        {
            ShadeMachine::Shader type;
            
            // check for valid source type, to discard errant files
            bool isVulkanSource = file.find(".vk") != std::string::npos;
            bool isHlslSource = file.find(".hlsl") != std::string::npos;
            
            // identify type of shader
            bool typeVertex = file.find("vertex") != std::string::npos;
            bool typeFragment = file.find("fragment") != std::string::npos;
            bool typeCompute = file.find("compute") != std::string::npos;
            
            if(!isVulkanSource && !isHlslSource)
            {
                continue;
            }
            
            if(typeVertex)
            {
                type = ShadeMachine::Vertex;
            }
            else if(typeFragment)
            {
                type = ShadeMachine::Fragment;
            }
            else if(typeCompute)
            {
                type = ShadeMachine::Compute;
            }
			else
			{
				continue;
			}
            
            // set, and fire.
            machine.SetOutputPath(result["output"].as<std::string>());
            
            machine.ProcessSource(file, type);
        }
    }
    catch (cxxopts::OptionException)
    {
        // couldn't parse valid arguments
        std::cerr << "Invalid Argument" << std::endl;
        return -100;
    }

}
